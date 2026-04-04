#include "player_controller.hpp"

#include "game_object.hpp"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <algorithm>

namespace prune {

    /**
     * @brief Updates a GameObject's transform from keyboard and mouse input and constrains it to the world bounds.
     *
     * Processes horizontal movement (Left/A, Right/D) and vertical movement (Up/W, Down/S) by applying
     * the controller's speed scaled by `dt`. If the left mouse button was pressed, repositions the object
     * so its rectangle is centered at the mouse position. Finally clamps the object's transform inside
     * the provided world width and height.
     *
     * @param object The game object to update; its transform and position will be modified.
     * @param dt Time step in seconds used to scale movement.
     * @param input Input state queried for key and mouse events.
     * @param world_width Width of the world area used for clamping the object's position (in pixels).
     * @param world_height Height of the world area used for clamping the object's position (in pixels).
     */
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

    /**
     * @brief Gets the controller's movement speed.
     *
     * @return The current movement speed value (guaranteed to be >= 0).
     */
    float PlayerController::speed() const noexcept
    {
        return m_speed;
    }

    /**
     * @brief Set the controller's movement speed.
     *
     * Updates the movement speed used for player motion. Negative values are clamped to zero.
     *
     * @param speed Desired speed in units per second; values less than 0 are treated as 0.
     */
    void PlayerController::set_speed(float speed) noexcept
    {
        m_speed = std::max(0.0f, speed);
    }

} // namespace prune