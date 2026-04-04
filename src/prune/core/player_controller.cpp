#include "player_controller.hpp"

#include "game_object.hpp"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

namespace prune {

    void PlayerController::update(
        GameObject& object,
        float dt,
        const Input& input,
        int world_width,
        int world_height
    ) const
    {
        float move_x = 0.0f;
        float move_y = 0.0f;

        if (input.is_key_down(SDL_SCANCODE_LEFT) || input.is_key_down(SDL_SCANCODE_A)) {
            move_x -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_RIGHT) || input.is_key_down(SDL_SCANCODE_D)) {
            move_x += 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_UP) || input.is_key_down(SDL_SCANCODE_W)) {
            move_y -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_DOWN) || input.is_key_down(SDL_SCANCODE_S)) {
            move_y += 1.0f;
        }

        if (move_x != 0.0f || move_y != 0.0f) {
            const float length = std::sqrt((move_x * move_x) + (move_y * move_y));
            object.transform.x += (move_x / length) * m_speed * dt;
            object.transform.y += (move_y / length) * m_speed * dt;
        }

        if (input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            const auto mouse_x = static_cast<float>(input.mouse_x());
            const auto mouse_y = static_cast<float>(input.mouse_y());
            const auto half_width = static_cast<float>(object.rectangle.width) * 0.5f;
            const auto half_height = static_cast<float>(object.rectangle.height) * 0.5f;

            object.transform.x = mouse_x - half_width;
            object.transform.y = mouse_y - half_height;
        }

        object.clamp_to_area(world_width, world_height);
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