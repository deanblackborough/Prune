#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/platformer/platformer_state.hpp"

namespace prune {

    class PlatformerBehaviour {
    public:
        void update(
            SceneState& state,
            SceneCamera& camera,
            PlatformerState& platformer_state,
            float dt,
            const Input& input,
            bool keyboard_input_enabled
        );

        void reset_player(SceneState& state, PlatformerState& platformer_state) const noexcept;

    private:
        [[nodiscard]] GameObject* player_object(SceneState& state, const PlatformerState& platformer_state) const noexcept;
        [[nodiscard]] const GameObject* player_object(const SceneState& state, const PlatformerState& platformer_state) const noexcept;
        [[nodiscard]] const GameObject* player_start_object(const SceneState& state, const PlatformerState& platformer_state) const noexcept;

        void update_player(SceneState& state, SceneCamera& camera, PlatformerState& platformer_state, float dt, const Input& input, bool keyboard_input_enabled) const;
        void move_player_axis(SceneState& state, GameObject& player, float delta_x, float delta_y, bool& grounded) const;
        [[nodiscard]] bool touches_hazard(const SceneState& state, const GameObject& player) const noexcept;
    };

}
