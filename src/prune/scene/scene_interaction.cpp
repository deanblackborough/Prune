#include "prune/scene/scene_interaction.hpp"

#include "prune/scene/scene.hpp"
#include "prune/editor/tools/transform_gizmo.hpp"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

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
            handle_delete_duplicate_shortcuts(scene, state, grid_options, input);
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
                        after,
                        "Mouse pan"
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

        const bool shift_down =
            input.is_key_down(SDL_SCANCODE_LSHIFT) ||
            input.is_key_down(SDL_SCANCODE_RSHIFT);

        if (picked && scene.object_is_selectable(*picked)) {
            if (shift_down) {
                state.objects.toggle_selected(picked->identity.id);
            } else {
                state.objects.select(picked->identity.id);
            }
            return;
        }

        state.objects.clear_selection();
    }

    void SceneInteraction::handle_object_drag(Scene& scene, SceneState& state, SceneCamera& camera, const GridOptions& grid_options, const Input& input)
    {
        if (state.drag_state.active) {
            if (!input.is_mouse_button_down(SDL_BUTTON_LEFT) || !state.viewport.has_area()) {
                std::vector<GameObject> before_objects;
                std::vector<GameObject> after_objects;

                before_objects.reserve(state.drag_state.object_starts.size());
                after_objects.reserve(state.drag_state.object_starts.size());

                for (const DragObjectStart& start : state.drag_state.object_starts) {
                    GameObject* object = state.objects.get_by_id(start.object_id);
                    if (!object) {
                        continue;
                    }

                    GameObject before = *object;
                    before.transform = start.transform;

                    if (before.transform.x == object->transform.x && before.transform.y == object->transform.y) {
                        continue;
                    }

                    before_objects.push_back(before);
                    after_objects.push_back(*object);
                }

                if (before_objects.size() == 1) {
                    scene.record_editor_command(make_object_command(
                        EditorCommandType::MoveObject,
                        editor_command_type_label(EditorCommandType::MoveObject),
                        before_objects.front(),
                        after_objects.front(),
                        "Mouse drag"
                    ));
                }
                else if (!before_objects.empty()) {
                    scene.record_editor_command(make_multi_object_command(
                        EditorCommandType::MoveObjects,
                        editor_command_type_label(EditorCommandType::MoveObjects),
                        before_objects,
                        after_objects,
                        std::to_string(before_objects.size()) + " objects, mouse drag"
                    ));
                }

                state.drag_state = {};
                return;
            }

            const Transform mouse_world = camera.screen_to_world(state.viewport, input.mouse_x(), input.mouse_y());
            const float delta_x = mouse_world.x - state.drag_state.mouse_start_world.x;
            const float delta_y = mouse_world.y - state.drag_state.mouse_start_world.y;

            for (const DragObjectStart& start : state.drag_state.object_starts) {
                GameObject* object = state.objects.get_by_id(start.object_id);
                if (!object || !scene.object_is_movable(*object)) {
                    continue;
                }

                object->transform.x = start.transform.x + delta_x;
                object->transform.y = start.transform.y + delta_y;

                if (grid_options.snap_to_grid) {
                    snap_object_to_grid(grid_options, *object);
                }
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

        const std::vector<GameObjectId> movable_ids = movable_objects_from_drag_start_at_screen(scene, state, camera, input.mouse_x(), input.mouse_y());
        if (movable_ids.empty()) {
            return;
        }

        state.drag_state = {};
        state.drag_state.active = true;
        state.drag_state.object_id = movable_ids.front();
        state.drag_state.mouse_start_world = camera.screen_to_world(state.viewport, input.mouse_x(), input.mouse_y());
        state.drag_state.object_starts.reserve(movable_ids.size());

        for (const GameObjectId id : movable_ids) {
            const GameObject* object = state.objects.get_by_id(id);
            if (!object) {
                continue;
            }

            state.drag_state.object_starts.push_back(DragObjectStart{ id, object->transform });
        }

        if (const GameObject* object = state.objects.get_by_id(state.drag_state.object_id)) {
            state.drag_state.object_start = object->transform;
        }
    }

    void SceneInteraction::handle_keyboard_nudge(Scene& scene, SceneState& state, const GridOptions& grid_options, const Input& input)
    {
        if (!scene_keyboard_input_enabled(state)) {
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

        if (state.objects.selected_count() == 0) {
            return;
        }

        if (state.objects.selected_count() > 1) {
            for (const GameObjectId id : state.objects.selected_ids()) {
                const GameObject* object = state.objects.get_by_id(id);
                if (!object || !scene.object_is_movable(*object)) {
                    return;
                }
            }
        }

        std::vector<GameObject> before_objects;
        std::vector<GameObject> after_objects;

        before_objects.reserve(state.objects.selected_count());
        after_objects.reserve(state.objects.selected_count());

        for (const GameObjectId id : state.objects.selected_ids()) {
            GameObject* object = state.objects.get_by_id(id);
            if (!object || !scene.object_is_movable(*object)) {
                continue;
            }

            const GameObject before = *object;

            object->transform.x += static_cast<float>(move_x * step);
            object->transform.y += static_cast<float>(move_y * step);

            if (grid_options.snap_to_grid) {
                snap_object_to_grid(grid_options, *object);
            }

            if (before.transform.x == object->transform.x && before.transform.y == object->transform.y) {
                continue;
            }

            before_objects.push_back(before);
            after_objects.push_back(*object);
        }

        if (before_objects.size() == 1) {
            scene.record_editor_command(make_object_command(
                EditorCommandType::MoveObject,
                editor_command_type_label(EditorCommandType::MoveObject),
                before_objects.front(),
                after_objects.front(),
                "Keyboard nudge"
            ));
        }
        else if (!before_objects.empty()) {
            scene.record_editor_command(make_multi_object_command(
                EditorCommandType::MoveObjects,
                editor_command_type_label(EditorCommandType::MoveObjects),
                before_objects,
                after_objects,
                std::to_string(before_objects.size()) + " objects, keyboard nudge"
            ));
        }
    }


    void SceneInteraction::handle_delete_duplicate_shortcuts(Scene& scene, SceneState& state, const GridOptions& grid_options, const Input& input)
    {
        if (!scene_keyboard_input_enabled(state)) {
            return;
        }

        const bool ctrl_down =
            input.is_key_down(SDL_SCANCODE_LCTRL) ||
            input.is_key_down(SDL_SCANCODE_RCTRL);

        if (input.was_key_pressed(SDL_SCANCODE_DELETE) || input.was_key_pressed(SDL_SCANCODE_BACKSPACE)) {
            std::vector<GameObject> deleted_objects;
            deleted_objects.reserve(state.objects.selected_count());

            for (const GameObjectId id : state.objects.selected_ids()) {
                const GameObject* object = state.objects.get_by_id(id);
                if (!object || !scene.object_is_editable(*object) || !object->editor.deletable) {
                    continue;
                }

                deleted_objects.push_back(*object);
            }

            if (deleted_objects.empty()) {
                return;
            }

            for (const GameObject& object : deleted_objects) {
                state.objects.remove_object(object.identity.id);
            }

            if (deleted_objects.size() > 1) {
                state.objects.clear_selection();
            }

            if (deleted_objects.size() == 1) {
                scene.record_editor_command(make_delete_object_command(deleted_objects.front(), deleted_objects.front().identity.name));
            }
            else {
                scene.record_editor_command(make_multi_delete_object_command(
                    deleted_objects,
                    std::to_string(deleted_objects.size()) + " objects"
                ));
            }
            return;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !scene.object_is_editable(*selected)) {
            return;
        }

        if (!selected->editor.cloneable || !ctrl_down || !input.was_key_pressed(SDL_SCANCODE_D)) {
            return;
        }

        GameObject duplicate = *selected;
        duplicate.identity.name = state.objects.make_unique_name(duplicate.identity.name + " Copy", k_invalid_game_object_id);

        const float offset = static_cast<float>(
            grid_options.snap_to_grid
                ? std::max(1, grid_options.grid_size)
                : std::max(1, grid_options.nudge_step)
        );

        duplicate.transform.x += offset;
        duplicate.transform.y += offset;

        const GameObjectId duplicate_id = state.objects.create_object(duplicate);
        if (GameObject* created = state.objects.get_by_id(duplicate_id)) {
            state.objects.select(duplicate_id);
            scene.record_editor_command(make_create_object_command(*created, "Duplicate object"));
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



    std::vector<GameObjectId> SceneInteraction::movable_objects_from_drag_start_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y)
    {
        if (editor_tool_allows_body_move(state.editor_tool)) {
            std::vector<GameObjectId> body_move_ids = movable_objects_from_move_tool_at_screen(scene, state, camera, screen_x, screen_y);
            if (!body_move_ids.empty()) {
                return body_move_ids;
            }
        }

        return movable_objects_from_handle_at_screen(scene, state, camera, screen_x, screen_y);
    }

    std::vector<GameObjectId> SceneInteraction::movable_objects_from_move_tool_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y)
    {
        GameObject* picked = pick_object_at_screen(scene, state, camera, screen_x, screen_y);
        if (!picked || !scene.object_is_movable(*picked)) {
            return {};
        }

        if (!state.objects.is_selected(picked->identity.id)) {
            state.objects.select(picked->identity.id);
            return { picked->identity.id };
        }

        if (state.objects.selected_count() <= 1) {
            return { picked->identity.id };
        }

        std::vector<GameObjectId> movable_ids;
        movable_ids.reserve(state.objects.selected_count());

        for (const GameObjectId id : state.objects.selected_ids()) {
            const GameObject* object = state.objects.get_by_id(id);
            if (!object || !scene.object_is_movable(*object)) {
                return {};
            }

            movable_ids.push_back(id);
        }

        return movable_ids;
    }

    std::vector<GameObjectId> SceneInteraction::movable_objects_from_handle_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y)
    {
        std::vector<GameObjectId> movable_ids;

        if (!state.scene_options.highlight_selected) {
            return movable_ids;
        }

        const int local_mouse_x = screen_x - state.viewport.screen_x;
        const int local_mouse_y = screen_y - state.viewport.screen_y;

        if (state.objects.selected_count() > 1) {
            SDL_Rect bounds{};
            if (!selected_screen_bounds(state, camera, bounds)) {
                return movable_ids;
            }

            const SDL_Rect outline{
                bounds.x - 6,
                bounds.y - 6,
                bounds.w + 12,
                bounds.h + 12
            };

            const SDL_Rect move_handle = editor::tools::transform_gizmo::move_handle_rect(outline);
            if (!editor::tools::transform_gizmo::contains_point(move_handle, local_mouse_x, local_mouse_y)) {
                return movable_ids;
            }

            movable_ids.reserve(state.objects.selected_count());
            for (const GameObjectId id : state.objects.selected_ids()) {
                const GameObject* object = state.objects.get_by_id(id);
                if (!object || !scene.object_is_movable(*object)) {
                    return {};
                }

                movable_ids.push_back(id);
            }

            return movable_ids;
        }

        GameObject* selected = state.objects.selected_object();
        if (!selected || !scene.object_is_movable(*selected)) {
            return movable_ids;
        }

        const SDL_Rect object_rect = camera.world_to_screen_rect(*selected);
        const SDL_Rect selected_outline = editor::tools::transform_gizmo::selected_outline_rect(object_rect);
        const SDL_Rect move_handle = editor::tools::transform_gizmo::move_handle_rect(selected_outline);

        if (!editor::tools::transform_gizmo::contains_point(move_handle, local_mouse_x, local_mouse_y)) {
            return movable_ids;
        }

        movable_ids.push_back(selected->identity.id);
        return movable_ids;
    }

    bool SceneInteraction::selected_screen_bounds(const SceneState& state, const SceneCamera& camera, SDL_Rect& bounds) noexcept
    {
        bool has_bounds = false;
        int min_x = 0;
        int min_y = 0;
        int max_x = 0;
        int max_y = 0;

        for (const GameObjectId id : state.objects.selected_ids()) {
            const GameObject* object = state.objects.get_by_id(id);
            if (!object || !object->lifecycle.active || !object->render.visible) {
                continue;
            }

            const SDL_Rect object_rect = camera.world_to_screen_rect(*object);
            if (!rect_visible(state.viewport, object_rect)) {
                continue;
            }

            const SDL_Rect selected_outline = editor::tools::transform_gizmo::selected_outline_rect(object_rect);

            if (!has_bounds) {
                min_x = selected_outline.x;
                min_y = selected_outline.y;
                max_x = selected_outline.x + selected_outline.w;
                max_y = selected_outline.y + selected_outline.h;
                has_bounds = true;
                continue;
            }

            min_x = std::min(min_x, selected_outline.x);
            min_y = std::min(min_y, selected_outline.y);
            max_x = std::max(max_x, selected_outline.x + selected_outline.w);
            max_y = std::max(max_y, selected_outline.y + selected_outline.h);
        }

        if (!has_bounds) {
            return false;
        }

        bounds = SDL_Rect{ min_x, min_y, max_x - min_x, max_y - min_y };
        return true;
    }

    bool SceneInteraction::rect_visible(const SceneViewport& viewport, const SDL_Rect& rect) noexcept
    {
        return rect.x + rect.w >= 0 &&
            rect.y + rect.h >= 0 &&
            rect.x < viewport.width &&
            rect.y < viewport.height;
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