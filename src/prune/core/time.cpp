#include "prune/core/time.hpp"
#include <SDL2/SDL.h>

namespace prune {

    Time::Time()
    {
        m_last_counter = SDL_GetPerformanceCounter();
    }

    void Time::tick()
    {
        const std::uint64_t current = SDL_GetPerformanceCounter();
        const std::uint64_t frequency = SDL_GetPerformanceFrequency();

        m_delta_seconds = static_cast<float>(current - m_last_counter) /
                          static_cast<float>(frequency);

        m_last_counter = current;
    }

}