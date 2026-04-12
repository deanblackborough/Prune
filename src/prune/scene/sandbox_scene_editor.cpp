#include <algorithm>
#include <string>

#include <SDL2/SDL.h>
#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/core/input.hpp"

namespace prune {

    GameObject* SandboxScene::pick_object_at_screen(int screen_x, int screen_y) noexcept
    {
        const Transform world = screen_to_world(screen_x, screen_y);
        auto& objects = m_objects.objects();

        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            GameObject& object = *it;

            if (!object.active || !object.visible) {
                continue;
            }

            const RectF bounds = object.bounds();

            const bool inside =
                world.x >= bounds.x &&
                world.x < (bounds.x + bounds.width) &&
                world.y >= bounds.y &&
                world.y < (bounds.y + bounds.height);

            if (inside) {
                return &object;
            }
        }

        return nullptr;
    }

    void SandboxScene::update_editor(float dt, const Input& input)
    {
        update_editor_camera(dt, input);
        handle_scene_click(input);
        handle_keyboard_nudge(input);
    }

    void SandboxScene::update_editor_camera(float dt, const Input& input)
    {
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }

        float move_x = 0.0f;
        float move_y = 0.0f;

        if (input.is_key_down(SDL_SCANCODE_J)) {
            move_x -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_L)) {
            move_x += 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_I)) {
            move_y -= 1.0f;
        }

        if (input.is_key_down(SDL_SCANCODE_K)) {
            move_y += 1.0f;
        }

        if (move_x == 0.0f && move_y == 0.0f) {
            return;
        }

        const float move_amount = camera_speed * dt;
        camera_x += move_x * move_amount;
        camera_y += move_y * move_amount;
    }

    void SandboxScene::handle_scene_click(const Input& input)
    {
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }

        if (!input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            return;
        }

        GameObject* clicked = pick_object_at_screen(input.mouse_x(), input.mouse_y());
        if (!clicked) {
            return;
        }

        m_objects.select(clicked->id);
    }

    void SandboxScene::handle_keyboard_nudge(const Input& input)
    {
        if (ImGui::GetIO().WantCaptureKeyboard) {
            return;
        }

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            return;
        }

        // Arrow key nudging is for editor objects only, not the player.
        if (selected->is_player) {
            return;
        }

        int move_x = 0;
        int move_y = 0;

        if (input.was_key_pressed(SDL_SCANCODE_LEFT)) {
            move_x -= 1;
        }

        if (input.was_key_pressed(SDL_SCANCODE_RIGHT)) {
            move_x += 1;
        }

        if (input.was_key_pressed(SDL_SCANCODE_UP)) {
            move_y -= 1;
        }

        if (input.was_key_pressed(SDL_SCANCODE_DOWN)) {
            move_y += 1;
        }

        if (move_x == 0 && move_y == 0) {
            return;
        }

        int step = m_grid_options.snap_to_grid
            ? std::max(1, m_grid_options.grid_size)
            : std::max(1, m_grid_options.nudge_step);

        const bool shift_down =
            input.is_key_down(SDL_SCANCODE_LSHIFT) ||
            input.is_key_down(SDL_SCANCODE_RSHIFT);

        if (shift_down) {
            step *= m_grid_options.shift_nudge_steps;
        }

        move_object(
            *selected,
            static_cast<float>(move_x * step),
            static_cast<float>(move_y * step),
            false
        );

        if (m_grid_options.snap_to_grid) {
            snap_object_to_grid(*selected);
        }
    }
} // namespace prune