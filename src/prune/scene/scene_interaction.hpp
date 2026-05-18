#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class SceneInteraction {
    public:
        void update(SceneState& state, SceneCamera& camera, const GridOptions& grid_options, float dt, const Input& input);

        // Clears any in-progress drag. Call when resetting or switching scenes.
        void reset() noexcept { m_drag_state = {}; }

    private:
        // Drag state is an implementation detail of interaction, not of SceneState.
        struct DragState {
            bool active = false;
            GameObjectId object_id = k_invalid_game_object_id;
            Transform object_start{};
            Transform mouse_start_world{};
        };

        [[nodiscard]] static bool scene_keyboard_input_enabled(const SceneState& state) noexcept;
        [[nodiscard]] static bool scene_mouse_input_enabled(const SceneState& state) noexcept;

        void update_editor_camera(SceneState& state, SceneCamera& camera, float dt, const Input& input);
        void handle_scene_click(SceneState& state, const SceneCamera& camera, const Input& input);
        void handle_object_drag(SceneState& state, SceneCamera& camera, const GridOptions& grid_options, const Input& input);
        void handle_keyboard_nudge(SceneState& state, const GridOptions& grid_options, const Input& input);

        [[nodiscard]] static GameObject* pick_object_at_screen(SceneState& state, const SceneCamera& camera, int screen_x, int screen_y) noexcept;

        [[nodiscard]] static float snap_value_to_grid(
            const GridOptions& grid_options,
            float value
        ) noexcept;

        static void snap_object_to_grid(
            const GridOptions& grid_options,
            GameObject& object
        ) noexcept;

        DragState m_drag_state{};
    };
}
