#include "time.hpp"
#include <SDL2/SDL.h>
#include <algorithm>

namespace prune {

    Time::Time()
        : m_start_ticks(SDL_GetPerformanceCounter())
        , m_last_ticks(m_start_ticks)
        , m_frame_count(0)
        , m_delta_time(0.0f)
        , m_elapsed_time(0.0f)
        , m_accumulator(0.0f)
    {
    }

    void Time::tick() {
        const uint64_t current_ticks = SDL_GetPerformanceCounter();
        const uint64_t delta_ticks = current_ticks - m_last_ticks;
        m_last_ticks = current_ticks;

        const uint64_t frequency = SDL_GetPerformanceFrequency();
        const float delta = static_cast<float>(delta_ticks) / static_cast<float>(frequency);

        m_delta_time = std::min(delta, MAX_DELTA_TIME);
        m_elapsed_time += m_delta_time;
        m_frame_count++;
    }

} // namespace prune