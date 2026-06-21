#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

#include <vector>

namespace prune {

    class Scene;

    class SceneInteraction {
    public:
        void update(
            Scene& scene,
            SceneState& state,
            SceneCamera& camera,
            const GridOptions& grid_options,
            float dt,
            const Input& input
        );

    private:
        [[nodiscard]] static bool scene_keyboard_input_enabled(const SceneState& state) noexcept;
        [[nodiscard]] static bool scene_mouse_input_enabled(const SceneState& state) noexcept;

        void update_editor_camera(Scene& scene, SceneState& state, SceneCamera& camera, float dt, const Input& input);
        void handle_scene_click(Scene& scene, SceneState& state, const SceneCamera& camera, const Input& input);
        void handle_object_drag(Scene& scene, SceneState& state, SceneCamera& camera, const GridOptions& grid_options, const Input& input);
        void handle_keyboard_nudge(Scene& scene, SceneState& state, const GridOptions& grid_options, const Input& input);
        void handle_delete_duplicate_shortcuts(Scene& scene, SceneState& state, const GridOptions& grid_options, const Input& input);

        [[nodiscard]] GameObject* pick_object_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y) noexcept;
        [[nodiscard]] std::vector<GameObjectId> movable_objects_from_drag_start_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y);
        [[nodiscard]] std::vector<GameObjectId> movable_objects_from_move_tool_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y);
        [[nodiscard]] std::vector<GameObjectId> movable_objects_from_handle_at_screen(Scene& scene, SceneState& state, const SceneCamera& camera, int screen_x, int screen_y);
        [[nodiscard]] static bool selected_screen_bounds(const SceneState& state, const SceneCamera& camera, SDL_Rect& bounds) noexcept;
        [[nodiscard]] static bool rect_visible(const SceneViewport& viewport, const SDL_Rect& rect) noexcept;

        [[nodiscard]] static float snap_value_to_grid(
            const GridOptions& grid_options,
            float value
        ) noexcept;

        static void snap_object_to_grid(
            const GridOptions& grid_options,
            GameObject& object
        ) noexcept;

        bool m_viewport_pan_active = false;
        Camera m_viewport_pan_start{};
    };
}
