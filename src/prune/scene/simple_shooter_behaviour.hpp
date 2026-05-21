#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/simple_shooter_state.hpp"

namespace prune {

    class SimpleShooterBehaviour {
    public:
        void update(
            SceneState& state,
            SceneCamera& camera,
            SimpleShooterState& shooter_state,
            float dt,
            const Input& input,
            bool keyboard_input_enabled
        );

        void reset(SceneState& state, SimpleShooterState& shooter_state);

        [[nodiscard]] int projectile_count(const SceneState& state) const noexcept;
        [[nodiscard]] int enemy_count(const SceneState& state) const noexcept;

        [[nodiscard]] GameObject* enemy_object(
            SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

        [[nodiscard]] const GameObject* enemy_object(
            const SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

        [[nodiscard]] GameObject* enemy_spawn_object(
            SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

        [[nodiscard]] const GameObject* enemy_spawn_object(
            const SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

    private:
        GameObject* player_object(
            SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

        const GameObject* player_object(
            const SceneState& state,
            const SimpleShooterState& shooter_state
        ) const noexcept;

        void update_player(
            SceneState& state,
            SceneCamera& camera,
            SimpleShooterState& shooter_state,
            float dt,
            const Input& input,
            bool keyboard_input_enabled
        );

        void move_object(SceneState& state, GameObject& object, float delta_x, float delta_y, bool resolve_wall_collisions);
        void update_player_facing(GameObject& player) const noexcept;

        void handle_player_shooting(
            SceneState& state,
            SimpleShooterState& shooter_state,
            const Input& input,
            bool keyboard_input_enabled
        );

        void update_enemies(SceneState& state, const SimpleShooterState& shooter_state, float dt);
        void update_projectiles(SceneState& state, float dt);
        void handle_projectile_wall_collisions(SceneState& state);
        void handle_projectile_enemy_collisions(SceneState& state);
        void ensure_enemy_count(SceneState& state, const SimpleShooterState& shooter_state);
        void create_runtime_enemy(SceneState& state, const SimpleShooterState& shooter_state);
        void respawn_enemy(SceneState& state, const SimpleShooterState& shooter_state, GameObject& enemy) const noexcept;
        void cleanup_runtime_objects(SceneState& state);
    };

}
