#include "prune/scene/scene_interaction.hpp"

#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

namespace prune {

    namespace {

        [[nodiscard]] Camera& active_camera(SceneState& state) noexcept
        {
            return state.cameras.mode == CameraMode::Editor
                ? state.cameras.editor
                : state.cameras.game;
        }

        [[nodiscard]] const Camera& active_camera(const SceneState& state) noexcept
        {
            return state.cameras.mode == CameraMode::Editor
                ? state.cameras.editor
                : state.cameras.game;
        }

    }

    void SceneInteraction::update(SceneState& state, float dt, const Input& input)
    {
        handle_object_drag(state, input);

        if (!state.drag_state.active) {
            update_editor_camera(state, dt, input);
            handle_scene_click(state, input);
            handle_keyboard_nudge(state, input);
        }
    }

    bool SceneInteraction::scene_keyboard_input_enabled(const SceneState& state) noexcept
    {
        return state.viewport.focused && state.viewport.has_area();
    }

    bool SceneInteraction::scene_mouse_input_enabled(const SceneState& state) noexcept
    {
        return state.viewport.hovered && state.viewport.has_area();
    }

    void SceneInteraction::update_editor_camera(SceneState& state, float, const Input& input)
    {
        if (!scene_mouse_input_enabled(state)) {
            return;
        }

        if (!input.is_mouse_button_down(SDL_BUTTON_MIDDLE)) {
            return;
        }

        state.cameras.mode = CameraMode::Editor;

        Camera& camera = state.cameras.editor;
        const float zoom = std::max(camera.zoom, 0.01f);

        camera.x -= static_cast<float>(input.mouse_delta_x()) / zoom;
        camera.y -= static_cast<float>(input.mouse_delta_y()) / zoom;
    }

    void SceneInteraction::handle_scene_click(SceneState& state, const Input& input)
    {
        if (!scene_mouse_input_enabled(state)) {
            return;
        }

        if (!input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            return;
        }

        if (!state.viewport.contains(input.mouse_x(), input.mouse_y())) {
            return;
        }

        GameObject* picked = pick_object_at_screen(state, input.mouse_x(), input.mouse_y());

        if (picked && picked->editor.selectable) {
            state.objects.select(picked->id);
            return;
        }

        state.objects.set_selected_id(k_invalid_game_object_id);
    }

    void SceneInteraction::handle_object_drag(SceneState& state, const Input& input)
    {
        if (state.drag_state.active) {
            if (!input.is_mouse_button_down(SDL_BUTTON_LEFT) || !state.viewport.has_area()) {
                state.drag_state = {};
                return;
            }

            GameObject* object = state.objects.get_by_id(state.drag_state.object_id);
            if (!object || !object->editor.movable) {
                state.drag_state = {};
                return;
            }

            const Transform mouse_world = screen_to_world(state, input.mouse_x(), input.mouse_y());

            object->transform.x =
                state.drag_state.object_start.x +
                (mouse_world.x - state.drag_state.mouse_start_world.x);

            object->transform.y =
                state.drag_state.object_start.y +
                (mouse_world.y - state.drag_state.mouse_start_world.y);

            if (state.grid_options.snap_to_grid) {
                snap_object_to_grid(state, *object);
            }

            return;
        }

        if (!scene_mouse_input_enabled(state)) {
            return;
        }

        if (!input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            return;
        }

        if (!state.viewport.contains(input.mouse_x(), input.mouse_y())) {
            return;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !selected->editor.movable) {
            return;
        }

        GameObject* picked = pick_object_at_screen(state, input.mouse_x(), input.mouse_y());
        if (!picked || picked->id != selected->id) {
            return;
        }

        state.drag_state.active = true;
        state.drag_state.object_id = selected->id;
        state.drag_state.object_start = selected->transform;
        state.drag_state.mouse_start_world = screen_to_world(state, input.mouse_x(), input.mouse_y());
    }

    void SceneInteraction::handle_keyboard_nudge(SceneState& state, const Input& input)
    {
        if (!scene_keyboard_input_enabled(state)) {
            return;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !selected->editor.movable) {
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

        int step = state.grid_options.snap_to_grid
            ? std::max(1, state.grid_options.grid_size)
            : std::max(1, state.grid_options.nudge_step);

        const bool shift_down =
            input.is_key_down(SDL_SCANCODE_LSHIFT) ||
            input.is_key_down(SDL_SCANCODE_RSHIFT);

        if (shift_down) {
            step *= state.grid_options.shift_nudge_steps;
        }

        selected->transform.x += static_cast<float>(move_x * step);
        selected->transform.y += static_cast<float>(move_y * step);

        if (state.grid_options.snap_to_grid) {
            snap_object_to_grid(state, *selected);
        }
    }

    GameObject* SceneInteraction::pick_object_at_screen(SceneState& state, int screen_x, int screen_y) noexcept
    {
        const Transform world = screen_to_world(state, screen_x, screen_y);
        auto& objects = state.objects.objects();

        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            GameObject& object = *it;

            if (!object.active || !object.render.visible || !object.editor.selectable) {
                continue;
            }

            const RectF bounds = object.bounds();

            const bool inside =
                world.x >= bounds.x &&
                world.x < bounds.x + bounds.width &&
                world.y >= bounds.y &&
                world.y < bounds.y + bounds.height;

            if (inside) {
                return &object;
            }
        }

        return nullptr;
    }

    Transform SceneInteraction::screen_to_world(const SceneState& state, int screen_x, int screen_y) noexcept
    {
        const Camera& camera = active_camera(state);
        const float zoom = std::max(camera.zoom, 0.01f);

        const int local_x = screen_x - state.viewport.screen_x;
        const int local_y = screen_y - state.viewport.screen_y;

        return {
            camera.x + static_cast<float>(local_x) / zoom,
            camera.y + static_cast<float>(local_y) / zoom
        };
    }

    float SceneInteraction::snap_value_to_grid(const SceneState& state, float value) noexcept
    {
        const int scene_grid_size = std::max(1, state.grid_options.grid_size);

        return std::round(value / static_cast<float>(scene_grid_size)) *
            static_cast<float>(scene_grid_size);
    }

    void SceneInteraction::snap_object_to_grid(const SceneState& state, GameObject& object) noexcept
    {
        object.transform.x = snap_value_to_grid(state, object.transform.x);
        object.transform.y = snap_value_to_grid(state, object.transform.y);
    }

}