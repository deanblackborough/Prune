#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class SimpleShooterBehaviour {
    public:
        void update(SceneState& state, float dt, const Input& input, bool keyboard_input_enabled);
        void reset(SceneState& state);

        [[nodiscard]] int bullet_count(const SceneState& state) const noexcept;

        [[nodiscard]] GameObject* enemy_object(SceneState& state) const noexcept;
        [[nodiscard]] const GameObject* enemy_object(const SceneState& state) const noexcept;

    private:
        [[nodiscard]] GameObject* player_object(SceneState& state) const noexcept;
        [[nodiscard]] const GameObject* player_object(const SceneState& state) const noexcept;

        void update_player(SceneState& state, float dt, const Input& input, bool keyboard_input_enabled);
        void move_object(SceneState& state, GameObject& object, float delta_x, float delta_y, bool resolve_collisions);
        void update_player_facing(GameObject& player) const noexcept;

        void handle_player_shooting(SceneState& state, const Input& input, bool keyboard_input_enabled);
        void update_enemy(SceneState& state, float dt);
        void update_bullets(SceneState& state, float dt);
        void handle_bullet_enemy_collisions(SceneState& state);
        void respawn_enemy(GameObject& enemy) const noexcept;
        void cleanup_runtime_objects(SceneState& state);
    };

}
