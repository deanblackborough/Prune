#include "player_controller.hpp"

#include "game_object.hpp"
#include "../core/input.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace prune {

    Velocity PlayerController::movement_velocity(const Input& input) const
    {
        float move_x = 0.0f;
        float move_y = 0.0f;

        if (input.is_key_down(SDL_SCANCODE_A)) {
            move_x -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_D)) {
            move_x += 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_W)) {
            move_y -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_S)) {
            move_y += 1.0f;
        }

        Velocity velocity{};

        if (move_x != 0.0f || move_y != 0.0f) {
            const float length = std::sqrt((move_x * move_x) + (move_y * move_y));

            velocity.x = (move_x / length) * m_speed;
            velocity.y = (move_y / length) * m_speed;
        }

        return velocity;
    }

    float PlayerController::speed() const noexcept
    {
        return m_speed;
    }

    void PlayerController::set_speed(float speed) noexcept
    {
        m_speed = std::max(0.0f, speed);
    }

} // namespace prune