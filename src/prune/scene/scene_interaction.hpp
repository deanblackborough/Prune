#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class SceneInteraction {
    public:
        void update(SceneState& state, float dt, const Input& input);

    private:
        [[nodiscard]] static bool scene_keyboard_input_enabled(const SceneState& state) noexcept;
        [[nodiscard]] static bool scene_mouse_input_enabled(const SceneState& state) noexcept;

        void update_editor_camera(SceneState& state, float dt, const Input& input);
        void handle_scene_click(SceneState& state, const Input& input);
        void handle_object_drag(SceneState& state, const Input& input);
        void handle_keyboard_nudge(SceneState& state, const Input& input);

        [[nodiscard]] GameObject* pick_object_at_screen(SceneState& state, int screen_x, int screen_y) noexcept;

        [[nodiscard]] static float snap_value_to_grid(
            const SceneState& state,
            float value
        ) noexcept;

        static void snap_object_to_grid(
            const SceneState& state,
            GameObject& object
        ) noexcept;
    };
}
