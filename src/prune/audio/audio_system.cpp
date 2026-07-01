#include "prune/audio/audio_system.hpp"

#include <algorithm>
#include <cstring>
#include <utility>

namespace prune {

    namespace {
        constexpr int k_audio_frequency = 48'000;
        constexpr int k_audio_channels = 2;
        constexpr int k_audio_sample_buffer = 1024;
        constexpr std::size_t k_max_active_voices = 16;
    }

    AudioSystem::~AudioSystem()
    {
        shutdown();
    }

    bool AudioSystem::initialise(std::string& error)
    {
        if (m_device_id != 0) {
            return true;
        }

        if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
            if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
                error = SDL_GetError();
                return false;
            }

            m_owns_audio_subsystem = true;
        }

        SDL_AudioSpec desired{};
        desired.freq = k_audio_frequency;
        desired.format = AUDIO_F32SYS;
        desired.channels = k_audio_channels;
        desired.samples = k_audio_sample_buffer;
        desired.callback = &AudioSystem::audio_callback;
        desired.userdata = this;

        m_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired, &m_device_spec, 0);
        if (m_device_id == 0) {
            error = SDL_GetError();

            if (m_owns_audio_subsystem) {
                SDL_QuitSubSystem(SDL_INIT_AUDIO);
                m_owns_audio_subsystem = false;
            }

            return false;
        }

        return true;
    }

    void AudioSystem::shutdown() noexcept
    {
        if (m_device_id != 0) {
            SDL_LockAudioDevice(m_device_id);
            m_voices.clear();
            SDL_UnlockAudioDevice(m_device_id);

            SDL_CloseAudioDevice(m_device_id);
            m_device_id = 0;
        }
        else {
            m_voices.clear();
        }

        m_sounds.clear();
        m_sound_indices.clear();
        m_event_sound_map.clear();

        if (m_owns_audio_subsystem) {
            SDL_QuitSubSystem(SDL_INIT_AUDIO);
            m_owns_audio_subsystem = false;
        }
    }

    void AudioSystem::start() noexcept
    {
        if (m_device_id != 0) {
            SDL_PauseAudioDevice(m_device_id, 0);
        }
    }

    bool AudioSystem::load_sound(
        std::string_view sound_id,
        const std::filesystem::path& path,
        std::string& error
    )
    {
        if (sound_id.empty()) {
            error = "Sound id cannot be empty.";
            return false;
        }

        if (m_device_id == 0) {
            error = "Audio device is not initialised.";
            return false;
        }

        if (m_sound_indices.contains(std::string(sound_id))) {
            return true;
        }

        SDL_AudioSpec source_spec{};
        Uint8* source_buffer = nullptr;
        Uint32 source_length = 0;

        if (SDL_LoadWAV(path.string().c_str(), &source_spec, &source_buffer, &source_length) == nullptr) {
            error = SDL_GetError();
            return false;
        }

        SDL_AudioCVT converter{};
        if (SDL_BuildAudioCVT(
            &converter,
            source_spec.format,
            source_spec.channels,
            source_spec.freq,
            m_device_spec.format,
            m_device_spec.channels,
            m_device_spec.freq
        ) < 0) {
            error = SDL_GetError();
            SDL_FreeWAV(source_buffer);
            return false;
        }

        converter.len = static_cast<int>(source_length);
        converter.buf = static_cast<Uint8*>(SDL_malloc(source_length * static_cast<Uint32>(converter.len_mult)));

        if (converter.buf == nullptr) {
            error = "Could not allocate audio conversion buffer.";
            SDL_FreeWAV(source_buffer);
            return false;
        }

        std::memcpy(converter.buf, source_buffer, source_length);
        SDL_FreeWAV(source_buffer);

        if (SDL_ConvertAudio(&converter) < 0) {
            error = SDL_GetError();
            SDL_free(converter.buf);
            return false;
        }

        SoundResource sound;
        sound.id = std::string(sound_id);
        sound.buffer.assign(converter.buf, converter.buf + converter.len_cvt);
        SDL_free(converter.buf);

        SDL_LockAudioDevice(m_device_id);

        const std::size_t index = m_sounds.size();
        m_sound_indices.emplace(sound.id, index);
        m_sounds.push_back(std::move(sound));

        SDL_UnlockAudioDevice(m_device_id);
        return true;
    }

    void AudioSystem::map_event_to_sound(std::string_view event_id, std::string_view sound_id)
    {
        if (event_id.empty() || sound_id.empty()) {
            return;
        }

        m_event_sound_map.insert_or_assign(std::string(event_id), std::string(sound_id));
    }

    void AudioSystem::handle_events(std::span<const SceneEvent> events)
    {
        if (!m_enabled.load() || m_device_id == 0 || events.empty()) {
            return;
        }

        for (const SceneEvent& event : events) {
            play_event(event.id);
        }
    }

    void AudioSystem::play_event(std::string_view event_id)
    {
        if (!m_enabled.load() || m_device_id == 0 || event_id.empty()) {
            return;
        }

        const auto event_match = m_event_sound_map.find(std::string(event_id));
        if (event_match == m_event_sound_map.end()) {
            return;
        }

        const auto sound_match = m_sound_indices.find(event_match->second);
        if (sound_match == m_sound_indices.end()) {
            return;
        }

        SDL_LockAudioDevice(m_device_id);

        if (m_voices.size() >= k_max_active_voices) {
            m_voices.erase(m_voices.begin());
        }

        m_voices.push_back(ActiveVoice{ sound_match->second, 0 });
        SDL_UnlockAudioDevice(m_device_id);
    }

    void AudioSystem::set_enabled(bool enabled) noexcept
    {
        m_enabled.store(enabled);

        if (!enabled) {
            stop_all_voices();
        }
    }

    void AudioSystem::audio_callback(void* userdata, std::uint8_t* stream, int length) noexcept
    {
        auto* audio = static_cast<AudioSystem*>(userdata);
        if (audio == nullptr) {
            std::memset(stream, 0, static_cast<std::size_t>(std::max(length, 0)));
            return;
        }

        audio->mix(stream, length);
    }

    void AudioSystem::mix(std::uint8_t* stream, int length) noexcept
    {
        if (stream == nullptr || length <= 0) {
            return;
        }

        std::memset(stream, 0, static_cast<std::size_t>(length));

        if (!m_enabled.load() || m_voices.empty()) {
            return;
        }

        for (ActiveVoice& voice : m_voices) {
            if (voice.sound_index >= m_sounds.size()) {
                continue;
            }

            const SoundResource& sound = m_sounds[voice.sound_index];
            if (voice.position >= sound.buffer.size()) {
                continue;
            }

            const std::uint32_t remaining = static_cast<std::uint32_t>(sound.buffer.size() - voice.position);
            const std::uint32_t bytes_to_mix = std::min(static_cast<std::uint32_t>(length), remaining);

            SDL_MixAudioFormat(
                stream,
                sound.buffer.data() + voice.position,
                m_device_spec.format,
                bytes_to_mix,
                SDL_MIX_MAXVOLUME
            );

            voice.position += bytes_to_mix;
        }

        std::erase_if(m_voices, [this](const ActiveVoice& voice) {
            return voice.sound_index >= m_sounds.size() ||
                voice.position >= m_sounds[voice.sound_index].buffer.size();
        });
    }

    void AudioSystem::stop_all_voices() noexcept
    {
        if (m_device_id == 0) {
            m_voices.clear();
            return;
        }

        SDL_LockAudioDevice(m_device_id);
        m_voices.clear();
        SDL_UnlockAudioDevice(m_device_id);
    }

}
