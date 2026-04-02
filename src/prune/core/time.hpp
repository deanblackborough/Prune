#pragma once

#include <cstdint>

namespace prune {

    class Time {
    public:
        Time();

        void tick();

        float delta_time() const { return m_delta_time; }
        float elapsed_time() const { return m_elapsed_time; }
        uint64_t frame_count() const { return m_frame_count; }

        float& accumulator() { return m_accumulator; }
        float accumulator() const { return m_accumulator; }

    private:
        uint64_t m_start_ticks;
        uint64_t m_last_ticks;
        uint64_t m_frame_count;
        float m_delta_time;
        float m_elapsed_time;
        float m_accumulator;

        static constexpr float MAX_DELTA_TIME = 0.1f;
    };

} // namespace prune