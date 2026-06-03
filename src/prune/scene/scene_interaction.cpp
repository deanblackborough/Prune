#include "prune/scene/scene_interaction.hpp"

#include "prune/scene/scene.hpp"
#include "prune/editor/tools/transform_gizmo.hpp"

#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

namespace prune {

    void SceneInteraction::update(
        Scene& scene,
        SceneState& state,
        SceneCamera& camera,
        const GridOptions& grid_options,
        float dt,
        const Input& input
    )
    {
        handle_object_drag(scene, state, camera, grid_options, input);

        if (!state.drag_state.active) {
            update_editor_camera(scene, state, camera, dt, input);
            handle_scene_click(scene, state, camera, input);
            handle_keyboard_nudge(scene, state, grid_options, input);
        }
    }

    bool SceneInteraction::scene_keyboard_input_enabled(const SceneState& state) noexcept
    {
        return state.viewport.keyboard_input_enabled && state.viewport.has_area();
    }

    bool SceneInteraction::scene_mouse_input_enabled(const SceneState& state) noexcept
    {
        return state.viewport.hovered && state.viewport.has_area();
    }

    void SceneInteraction::update_editor_camera(Scene& scene, SceneState& state, SceneCamera& camera, float, const Input& input)
    {
        if (!scene_mouse_input_enabled(state)) {
            if (m_viewport_pan_active) {
                m_viewport_pan_active = false;
            }
            return;
        }

        if (!input.is_mouse_button_down(SDL_BUTTON_MIDDLE)) {
            if (m_viewport_pan_active) {
                const Camera after = camera.editor();
                if (m_viewport_pan_start.x != after.x || m_viewport_pan_start.y != after.y) {
                    scene.record_editor_command(make_viewport_command(
                        editor_command_type_label(EditorCommandType::MoveViewport),
                        m_viewport_pan_start,
                        after
                    ));
                }

                m_viewport_pan_active = false;
            }
            return;
        }

        camera.activate_editor();

        if (!m_viewport_pan_active) {
            m_viewport_pan_active = true;
            m_viewport_pan_start = camera.editor();
        }

        camera.pan_editor_by_mouse_delta(input.mouse_delta_x(), input.mouse_delta_y());
    }

    void SceneInteraction::handle_scene_click(Scene& scene, SceneState& state, const SceneCamera& camera, const Input& input)
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

        GameObject* picked = pick_object_at_screen(scene, state, camera, input.mouse_x(), input.mouse_y());

        if (picked && scene.object_is_selectable(*picked)) {
            state.objects.select(picked->identity.id);
            return;
        }

        state.objects.set_selected_id(k_invalid_game_object_id);
    }

    void SceneInteraction::handle_object_drag(Scene& scene, SceneState& state, SceneCamera& camera, const GridOptions& grid_options, const Input& input)
    {
        if (state.drag_state.active) {
            if (!input.is_mouse_button_down(SDL_BUTTON_LEFT) || !state.viewport.has_area()) {
                if (GameObject* object = state.objects.get_by_id(state.drag_state.object_id)) {
                    GameObject before = *object;
                    before.transform = state.drag_state.object_start;

                    if (before.transform.x != object->transform.x || before.transform.y != object->transform.y) {
                        scene.record_editor_command(make_object_command(
                            EditorCommandType::MoveObject,
                            editor_command_type_label(EditorCommandType::MoveObject),
                            before,
                            *object
                        ));
                    }
                }

                state.drag_state = {};
                return;
            }

            GameObject* object = state.objects.get_by_id(state.drag_state.object_id);
            if (!object || !scene.object_is_movable(*object)) {
                state.drag_state = {};
                return;
            }

            const Transform mouse_world = camera.screen_to_world(state.viewport, input.mouse_x(), input.mouse_y());

            object->transform.x =
                state.drag_state.object_start.x +
                (mouse_world.x - state.drag_state.mouse_start_world.x);

            object->transform.y =
                state.drag_state.object_start.y +
                (mouse_world.y - state.drag_state.mouse_start_world.y);

            if (grid_options.snap_to_grid) {
                snap_object_to_grid(grid_options, *object);
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

        GameObject* selected = movable_object_from_handle_at_screen(scene, state, camera, input.mouse_x(), input.mouse_y());
        if (!selected) {
            return;
        }

        state.drag_state.active = true;
        state.drag_state.object_id = selected->identity.id;
        state.drag_state.object_start = selected->transform;
        state.drag_state.mouse_start_world = camera.screen_to_world(state.viewport, input.mouse_x(), input.mouse_y());
    }

    void SceneInteraction::handle_keyboard_nudge(Scene& scene, SceneState& state, const GridOptions& grid_options, const Input& input)
    {
        if (!scene_keyboard_input_enabled(state)) {
            return;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !scene.object_is_movable(*selected)) {
            return;
        }

        const bool ctrl_down =
            input.is_key_down(SDL_SCANCODE_LCTRL) ||
            input.is_key_down(SDL_SCANCODE_RCTRL);

        if (!ctrl_down) {
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

        int step = grid_options.snap_to_grid
            ? std::max(1, grid_options.grid_size)
            : std::max(1, grid_options.nudge_step);

        const bool shift_down =
            input.is_key_down(SDL_SCANCODE_LSHIFT) ||
            input.is_key_down(SDL_SCANCODE_RSHIFT);

        if (shift_down) {
            step *= grid_options.shift_nudge_steps;
        }

        const GameObject before = *selected;

        selected->transform.x += static_cast<float>(move_x * step);
        selected->transform.y += static_cast<float>(move_y * step);

        if (grid_options.snap_to_grid) {
            snap_object_to_grid(grid_options, *selected);
        }

        if (before.transform.x != selected->transform.x || before.transform.y != selected->transform.y) {
            scene.record_editor_command(make_object_command(
                EditorCommandType::MoveObject,
                editor_command_type_label(EditorCommandType::MoveObject),
                before,
                *selected
            ));
        }
    }

    GameObject* SceneInteraction::pick_object_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y) noexcept
    {
        const Transform world = camera.screen_to_world(state.viewport, screen_x, screen_y);
        auto& objects = state.objects.objects();

        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            GameObject& object = *it;

            if (!object.lifecycle.active || !object.render.visible || !scene.object_is_selectable(object)) {
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


    GameObject* SceneInteraction::movable_object_from_handle_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y) noexcept
    {
        if (!state.scene_options.highlight_selected) {
            return nullptr;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !scene.object_is_movable(*selected)) {
            return nullptr;
        }

        const SDL_Rect object_rect = camera.world_to_screen_rect(*selected);
        const SDL_Rect selected_outline = editor::tools::transform_gizmo::selected_outline_rect(object_rect);
        const SDL_Rect move_handle = editor::tools::transform_gizmo::move_handle_rect(selected_outline);

        const int local_mouse_x = screen_x - state.viewport.screen_x;
        const int local_mouse_y = screen_y - state.viewport.screen_y;

        if (!editor::tools::transform_gizmo::contains_point(move_handle, local_mouse_x, local_mouse_y)) {
            return nullptr;
        }

        return selected;
    }

    float SceneInteraction::snap_value_to_grid(const GridOptions& grid_options, float value) noexcept
    {
        const int scene_grid_size = std::max(1, grid_options.grid_size);

        return std::round(value / static_cast<float>(scene_grid_size)) *
            static_cast<float>(scene_grid_size);
    }

    void SceneInteraction::snap_object_to_grid(const GridOptions& grid_options, GameObject& object) noexcept
    {
        object.transform.x = snap_value_to_grid(grid_options, object.transform.x);
        object.transform.y = snap_value_to_grid(grid_options, object.transform.y);
    }

}