#pragma once

#include <atomic>
#include <cstdint>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <SDL2/SDL.h>

#include "prune/scene/scene_event.hpp"

namespace prune {

    namespace sound_ids {
        inline constexpr std::string_view shoot = "shoot";
        inline constexpr std::string_view jump = "jump";
        inline constexpr std::string_view explosion = "explosion";
    }

    class AudioSystem {
    public:
        AudioSystem() = default;
        ~AudioSystem();

        AudioSystem(const AudioSystem&) = delete;
        AudioSystem& operator=(const AudioSystem&) = delete;

        [[nodiscard]] bool initialise(std::string& error);
        void shutdown() noexcept;
        void start() noexcept;

        [[nodiscard]] bool load_sound(
            std::string_view sound_id,
            const std::filesystem::path& path,
            std::string& error
        );

        void map_event_to_sound(std::string_view event_id, std::string_view sound_id);
        void handle_events(std::span<const SceneEvent> events);
        void play_event(std::string_view event_id);

        void set_enabled(bool enabled) noexcept;
        [[nodiscard]] bool enabled() const noexcept { return m_enabled.load(); }
        [[nodiscard]] bool available() const noexcept { return m_device_id != 0; }

    private:
        struct SoundResource {
            std::string id;
            std::vector<std::uint8_t> buffer;
        };

        struct ActiveVoice {
            std::size_t sound_index = 0;
            std::uint32_t position = 0;
        };

        static void audio_callback(void* userdata, std::uint8_t* stream, int length) noexcept;
        void mix(std::uint8_t* stream, int length) noexcept;
        void stop_all_voices() noexcept;

        SDL_AudioSpec m_device_spec{};
        SDL_AudioDeviceID m_device_id = 0;
        bool m_owns_audio_subsystem = false;

        std::vector<SoundResource> m_sounds;
        std::vector<ActiveVoice> m_voices;
        std::unordered_map<std::string, std::size_t> m_sound_indices;
        std::unordered_map<std::string, std::string> m_event_sound_map;

        std::atomic_bool m_enabled = true;
    };

}
