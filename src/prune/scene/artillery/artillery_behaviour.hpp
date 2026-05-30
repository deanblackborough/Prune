#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/artillery/artillery_state.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class ArtilleryBehaviour {
    public:
        void update(SceneState& state, SceneCamera&, ArtilleryState& artillery_state, float dt, const Input& input, bool keyboard_input_enabled) const;
        void reset_round(SceneState& state, ArtilleryState& artillery_state) const;

    private:
        void update_controls(SceneState& state, ArtilleryState& artillery_state, float dt, const Input& input, bool keyboard_input_enabled) const;
        void update_projectile(SceneState& state, ArtilleryState& artillery_state, float dt) const;
        void fire_projectile(SceneState& state, ArtilleryState& artillery_state) const;
        void end_turn(SceneState& state, ArtilleryState& artillery_state) const noexcept;

        [[nodiscard]] GameObject* current_tank(SceneState& state, const ArtilleryState& artillery_state) const noexcept;
        [[nodiscard]] const GameObject* current_tank(const SceneState& state, const ArtilleryState& artillery_state) const noexcept;
        [[nodiscard]] bool projectile_hit_solid(const SceneState& state, const GameObject& projectile, GameObjectId ignored_object_id, GameObjectId& hit_id) const noexcept;
    };

}
