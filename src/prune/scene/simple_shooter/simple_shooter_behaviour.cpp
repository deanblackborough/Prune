#include <cmath>

#include <SDL2/SDL.h>

#include "prune/scene/collision.hpp"
#include "prune/scene/simple_shooter/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter/simple_shooter_concepts.hpp"
#include "prune/scene/simple_shooter/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter/simple_shooter_ids.hpp"

namespace prune {

    void SimpleShooterBehaviour::update(
        SceneState& state,
        SceneCamera& camera,
        SimpleShooterState& shooter_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        if (shooter_state.options.paused) {
            return;
        }

        if (shooter_state.fire_cooldown_remaining > 0.0f) {
            shooter_state.fire_cooldown_remaining -= dt;
        }

        update_player(state, camera, shooter_state, dt, input, keyboard_input_enabled);
        handle_player_shooting(state, shooter_state, input, keyboard_input_enabled);
        update_enemies(state, shooter_state, dt);
        update_projectiles(state, dt);
        handle_projectile_wall_collisions(state);
        handle_projectile_enemy_collisions(state);
        cleanup_runtime_objects(state);
        ensure_enemy_count(state, shooter_state);
    }

    void SimpleShooterBehaviour::reset(SceneState& state, SimpleShooterState& shooter_state)
    {
        shooter_state.fire_cooldown_remaining = 0.0f;

        for (auto& object : state.objects.objects()) {
            if (simple_shooter_concepts::is_projectile(object)) {
                object.lifecycle.active = false;
            }
        }

        cleanup_runtime_objects(state);

        if (!enemy_spawn_object(state, shooter_state)) {
            shooter_state.enemy_spawn_id = state.objects.create_object(simple_shooter_factory::create_enemy_spawn());
        }

        GameObject* enemy = enemy_object(state, shooter_state);
        if (!enemy) {
            shooter_state.enemy_id = state.objects.create_object(simple_shooter_factory::create_enemy());
            enemy = enemy_object(state, shooter_state);
        }

        if (enemy) {
            respawn_enemy(state, shooter_state, *enemy);
        }
    }

    int SimpleShooterBehaviour::projectile_count(const SceneState& state) const noexcept
    {
        int count = 0;

        for (const auto& object : state.objects.objects()) {
            if (simple_shooter_concepts::is_projectile(object) && object.lifecycle.active) {
                ++count;
            }
        }

        return count;
    }

    int SimpleShooterBehaviour::enemy_count(const SceneState& state) const noexcept
    {
        int count = 0;

        for (const auto& object : state.objects.objects()) {
            if (simple_shooter_concepts::is_enemy(object) && object.lifecycle.active) {
                ++count;
            }
        }

        return count;
    }

    GameObject* SimpleShooterBehaviour::enemy_object(
        SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.enemy_id);
    }

    const GameObject* SimpleShooterBehaviour::enemy_object(
        const SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.enemy_id);
    }

    GameObject* SimpleShooterBehaviour::enemy_spawn_object(
        SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.enemy_spawn_id);
    }

    const GameObject* SimpleShooterBehaviour::enemy_spawn_object(
        const SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.enemy_spawn_id);
    }

    GameObject* SimpleShooterBehaviour::player_object(
        SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.player_id);
    }

    const GameObject* SimpleShooterBehaviour::player_object(
        const SceneState& state,
        const SimpleShooterState& shooter_state
    ) const noexcept
    {
        return state.objects.get_by_id(shooter_state.player_id);
    }

    void SimpleShooterBehaviour::update_player(
        SceneState& state,
        SceneCamera& camera,
        SimpleShooterState& shooter_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        GameObject* player = player_object(state, shooter_state);
        if (!player) {
            return;
        }

        if (!keyboard_input_enabled) {
            player->motion.velocity = {};
            return;
        }

        player->motion.velocity = shooter_state.player_controller.movement_velocity(input);

        const bool is_moving =
            player->motion.velocity.x != 0.0f ||
            player->motion.velocity.y != 0.0f;

        if (is_moving) {
            camera.activate_game();
            update_player_facing(*player);
        }

        move_object(
            state,
            *player,
            player->motion.velocity.x * dt,
            player->motion.velocity.y * dt,
            true
        );
    }

    void SimpleShooterBehaviour::move_object(
        SceneState& state,
        GameObject& object,
        float delta_x,
        float delta_y,
        bool resolve_wall_collisions
    )
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_wall_collisions) {
            collision::resolve_against_solids(object, state.objects);
        }
    }

    void SimpleShooterBehaviour::update_player_facing(GameObject& player) const noexcept
    {
        if (std::abs(player.motion.velocity.x) > std::abs(player.motion.velocity.y)) {
            player.motion.facing = player.motion.velocity.x < 0.0f ? Direction::Left : Direction::Right;
            return;
        }

        if (player.motion.velocity.y != 0.0f) {
            player.motion.facing = player.motion.velocity.y < 0.0f ? Direction::Up : Direction::Down;
        }
    }

    void SimpleShooterBehaviour::handle_player_shooting(
        SceneState& state,
        SimpleShooterState& shooter_state,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        if (!keyboard_input_enabled) {
            return;
        }

        if (shooter_state.fire_cooldown_remaining > 0.0f || !input.is_key_down(SDL_SCANCODE_SPACE)) {
            return;
        }

        const GameObject* player = player_object(state, shooter_state);
        if (!player) {
            return;
        }

        state.objects.create_object(
            simple_shooter_factory::create_projectile_from_player(
                *player,
                shooter_state.options.projectile_speed,
                shooter_state.options.projectile_lifetime
            )
        );

        shooter_state.fire_cooldown_remaining = shooter_state.options.fire_cooldown;
    }

    void SimpleShooterBehaviour::update_enemies(
        SceneState& state,
        const SimpleShooterState& shooter_state,
        float dt
    )
    {
        const GameObject* player = player_object(state, shooter_state);
        if (!player) {
            return;
        }

        for (auto& enemy : state.objects.objects()) {
            if (!simple_shooter_concepts::is_enemy(enemy) || !enemy.lifecycle.active) {
                continue;
            }

            const float enemy_center_x = enemy.transform.x + (static_cast<float>(enemy.size.width) * 0.5f);
            const float enemy_center_y = enemy.transform.y + (static_cast<float>(enemy.size.height) * 0.5f);
            const float player_center_x = player->transform.x + (static_cast<float>(player->size.width) * 0.5f);
            const float player_center_y = player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

            float direction_x = player_center_x - enemy_center_x;
            float direction_y = player_center_y - enemy_center_y;
            const float length = std::sqrt((direction_x * direction_x) + (direction_y * direction_y));

            if (length <= 0.001f) {
                enemy.motion.velocity = {};
                continue;
            }

            direction_x /= length;
            direction_y /= length;

            enemy.motion.velocity.x = direction_x * shooter_state.options.enemy_speed;
            enemy.motion.velocity.y = direction_y * shooter_state.options.enemy_speed;

            move_object(
                state,
                enemy,
                enemy.motion.velocity.x * dt,
                enemy.motion.velocity.y * dt,
                true
            );
        }
    }

    void SimpleShooterBehaviour::update_projectiles(SceneState& state, float dt)
    {
        for (auto& object : state.objects.objects()) {
            if (!simple_shooter_concepts::is_projectile(object) || !object.lifecycle.active) {
                continue;
            }

            object.transform.x += object.motion.velocity.x * dt;
            object.transform.y += object.motion.velocity.y * dt;
            object.lifecycle.remaining -= dt;

            if (object.lifecycle.remaining <= 0.0f) {
                object.lifecycle.active = false;
            }
        }
    }

    void SimpleShooterBehaviour::handle_projectile_wall_collisions(SceneState& state)
    {
        for (auto& projectile : state.objects.objects()) {
            if (!simple_shooter_concepts::is_projectile(projectile) || !projectile.lifecycle.active) {
                continue;
            }

            for (const auto& wall : state.objects.objects()) {
                if (!simple_shooter_concepts::is_wall(wall) || !wall.lifecycle.active || !wall.collision.solid) {
                    continue;
                }

                if (collision::are_active_overlapping(projectile, wall)) {
                    projectile.lifecycle.active = false;
                    break;
                }
            }
        }
    }

    void SimpleShooterBehaviour::handle_projectile_enemy_collisions(SceneState& state)
    {
        for (auto& projectile : state.objects.objects()) {
            if (!simple_shooter_concepts::is_projectile(projectile) || !projectile.lifecycle.active) {
                continue;
            }

            for (auto& enemy : state.objects.objects()) {
                if (!simple_shooter_concepts::is_enemy(enemy) || !enemy.lifecycle.active) {
                    continue;
                }

                if (!collision::are_active_overlapping(projectile, enemy)) {
                    continue;
                }

                projectile.lifecycle.active = false;
                enemy.lifecycle.active = false;
                enemy.render.visible = false;
                enemy.motion.velocity = {};
                break;
            }
        }
    }

    void SimpleShooterBehaviour::ensure_enemy_count(
        SceneState& state,
        const SimpleShooterState& shooter_state
    )
    {
        const int target_enemy_count = shooter_state.options.max_live_enemies;
        int live_enemy_count = 0;

        for (auto& object : state.objects.objects()) {
            if (!simple_shooter_concepts::is_enemy(object) || !object.lifecycle.active) {
                continue;
            }

            if (live_enemy_count >= target_enemy_count) {
                object.lifecycle.active = false;
                object.render.visible = false;
                object.motion.velocity = {};
                continue;
            }

            ++live_enemy_count;
        }

        if (target_enemy_count <= 0) {
            return;
        }

        while (enemy_count(state) < target_enemy_count) {
            GameObject* inactive_enemy = nullptr;

            for (auto& object : state.objects.objects()) {
                if (simple_shooter_concepts::is_enemy(object) && !object.lifecycle.active) {
                    inactive_enemy = &object;
                    break;
                }
            }

            if (inactive_enemy) {
                respawn_enemy(state, shooter_state, *inactive_enemy);
                continue;
            }

            create_runtime_enemy(state, shooter_state);
        }
    }

    void SimpleShooterBehaviour::create_runtime_enemy(
        SceneState& state,
        const SimpleShooterState& shooter_state
    )
    {
        GameObject enemy = simple_shooter_factory::create_enemy();
        enemy.identity.name = state.objects.make_unique_name("Enemy", k_invalid_game_object_id);
        enemy.identity.type = GameObjectType::Runtime;
        enemy.runtime.persistent = false;
        enemy.editor.selectable = false;
        enemy.editor.renameable = false;
        enemy.editor.movable = false;
        enemy.editor.deletable = false;
        enemy.editor.cloneable = false;

        respawn_enemy(state, shooter_state, enemy);
        state.objects.create_object(enemy);
    }

    void SimpleShooterBehaviour::respawn_enemy(
        SceneState& state,
        const SimpleShooterState& shooter_state,
        GameObject& enemy
    ) const noexcept
    {
        const GameObject* spawn = enemy_spawn_object(state, shooter_state);

        if (spawn) {
            enemy.transform.x = spawn->transform.x;
            enemy.transform.y = spawn->transform.y;
        }
        else {
            enemy.transform.x = 256.0f;
            enemy.transform.y = 128.0f;
        }

        enemy.motion.velocity = {};
        enemy.lifecycle.active = true;
        enemy.render.visible = true;
    }

    void SimpleShooterBehaviour::cleanup_runtime_objects(SceneState& state)
    {
        state.objects.remove_inactive_runtime_objects(simple_shooter_ids::projectile_behaviour);
        state.objects.remove_inactive_runtime_objects(simple_shooter_ids::enemy_behaviour);
    }
}
