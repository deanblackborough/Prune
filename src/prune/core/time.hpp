#pragma once

#include <cstdint>

namespace prune {

    class Time {
    public:
        Time();

        void tick();

        [[nodiscard]] float delta_seconds() const { return m_delta_seconds; }

    private:
        std::uint64_t m_last_counter = 0;
        float m_delta_seconds = 0.0f;
    };

}