#include "player_controller.hpp"

#include "game_object.hpp"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <algorithm>

namespace prune {

    void PlayerController::update(
        GameObject& object,
        float dt,
        const Input& input,
        int world_width,
        int world_height
    ) const
    {
        if (input.is_key_down(SDL_SCANCODE_LEFT) || input.is_key_down(SDL_SCANCODE_A)) {
            object.transform.x -= m_speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_RIGHT) || input.is_key_down(SDL_SCANCODE_D)) {
            object.transform.x += m_speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_UP) || input.is_key_down(SDL_SCANCODE_W)) {
            object.transform.y -= m_speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_DOWN) || input.is_key_down(SDL_SCANCODE_S)) {
            object.transform.y += m_speed * dt;
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