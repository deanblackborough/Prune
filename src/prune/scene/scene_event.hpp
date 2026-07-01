#pragma once

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace prune {

    namespace scene_events {
        inline constexpr std::string_view player_fired = "player_fired";
        inline constexpr std::string_view player_jumped = "player_jumped";
        inline constexpr std::string_view player_hit = "player_hit";
        inline constexpr std::string_view enemy_destroyed = "enemy_destroyed";
        inline constexpr std::string_view round_reset = "round_reset";
    }

    struct SceneEvent {
        std::string id;
    };

    class SceneEventQueue {
    public:
        void emit(std::string_view event_id)
        {
            if (event_id.empty()) {
                return;
            }

            m_events.push_back(SceneEvent{ std::string(event_id) });
        }

        [[nodiscard]] std::span<const SceneEvent> pending() const noexcept
        {
            return std::span<const SceneEvent>{ m_events.data(), m_events.size() };
        }

        void clear() noexcept
        {
            m_events.clear();
        }

    private:
        std::vector<SceneEvent> m_events;
    };

}
