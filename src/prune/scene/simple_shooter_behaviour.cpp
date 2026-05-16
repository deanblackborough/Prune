#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include "prune/scene/collision.hpp"
#include "prune/scene/simple_shooter_ids.hpp"
#include "prune/scene/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter_factory.hpp"

namespace prune {

    void SimpleShooterBehaviour::update(
        SceneState& state,
        SimpleShooterState& shooter_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        if (shooter_state.options.paused) {
            return;
        }

        update_player(state, dt, input, keyboard_input_enabled);
        handle_player_shooting(state, shooter_state, input, keyboard_input_enabled);
        update_enemy(state, shooter_state, dt);
        update_bullets(state, dt);
        handle_bullet_enemy_collisions(state, shooter_state);
        cleanup_runtime_objects(state);
    }

    void SimpleShooterBehaviour::reset(SceneState& state, SimpleShooterState& shooter_state)
    {
        for (auto& object : state.objects.objects()) {
            if (object.runtime.behaviour == simple_shooter_ids::bullet_behaviour) {
                object.active = false;
            }
        }

        cleanup_runtime_objects(state);

        GameObject* enemy = enemy_object(state, shooter_state);
        if (!enemy) {
            shooter_state.enemy_id = state.objects.create_object(simple_shooter_factory::create_enemy());
            return;
        }

        respawn_enemy(*enemy);
    }

    int SimpleShooterBehaviour::bullet_count(const SceneState& state) const noexcept
    {
        int count = 0;

        for (const auto& object : state.objects.objects()) {
            if (object.runtime.behaviour == simple_shooter_ids::bullet_behaviour && object.active) {
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

    GameObject* SimpleShooterBehaviour::player_object(SceneState& state) const noexcept
    {
        return state.objects.get_by_id(state.player_id);
    }

    const GameObject* SimpleShooterBehaviour::player_object(const SceneState& state) const noexcept
    {
        return state.objects.get_by_id(state.player_id);
    }

    void SimpleShooterBehaviour::update_player(
        SceneState& state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        GameObject* player = player_object(state);
        if (!player) {
            return;
        }

        if (!keyboard_input_enabled) {
            player->velocity = {};
            return;
        }

        player->velocity = state.player_controller.movement_velocity(input);

        const bool is_moving =
            player->velocity.x != 0.0f ||
            player->velocity.y != 0.0f;

        if (is_moving) {
            state.camera.activate_game();
            update_player_facing(*player);
        }

        move_object(state, *player, player->velocity.x * dt, player->velocity.y * dt, true);
    }

    void SimpleShooterBehaviour::move_object(
        SceneState& state,
        GameObject& object,
        float delta_x,
        float delta_y,
        bool resolve_collisions
    )
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_collisions) {
            collision::resolve_against_solids(object, state.objects);
        }
    }

    void SimpleShooterBehaviour::update_player_facing(GameObject& player) const noexcept
    {
        if (std::abs(player.velocity.x) > std::abs(player.velocity.y)) {
            player.facing = player.velocity.x < 0.0f ? Direction::Left : Direction::Right;
            return;
        }

        if (player.velocity.y != 0.0f) {
            player.facing = player.velocity.y < 0.0f ? Direction::Up : Direction::Down;
        }
    }

    void SimpleShooterBehaviour::handle_player_shooting(
        SceneState& state,
        const SimpleShooterState& shooter_state,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        if (!keyboard_input_enabled) {
            return;
        }

        if (!input.was_key_pressed(SDL_SCANCODE_SPACE)) {
            return;
        }

        const GameObject* player = player_object(state);
        if (!player) {
            return;
        }

        state.objects.create_object(
            simple_shooter_factory::create_bullet_from_player(
                *player,
                shooter_state.options.bullet_speed,
                shooter_state.options.bullet_lifetime
            )
        );
    }

    void SimpleShooterBehaviour::update_enemy(
        SceneState& state,
        const SimpleShooterState& shooter_state,
        float dt
    )
    {
        GameObject* enemy = enemy_object(state, shooter_state);
        const GameObject* player = player_object(state);

        if (!enemy || !enemy->active || !player) {
            return;
        }

        const float enemy_center_x = enemy->transform.x + (static_cast<float>(enemy->size.width) * 0.5f);
        const float enemy_center_y = enemy->transform.y + (static_cast<float>(enemy->size.height) * 0.5f);
        const float player_center_x = player->transform.x + (static_cast<float>(player->size.width) * 0.5f);
        const float player_center_y = player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

        float direction_x = player_center_x - enemy_center_x;
        float direction_y = player_center_y - enemy_center_y;
        const float length = std::sqrt((direction_x * direction_x) + (direction_y * direction_y));

        if (length <= 0.001f) {
            enemy->velocity = {};
            return;
        }

        direction_x /= length;
        direction_y /= length;

        enemy->velocity.x = direction_x * shooter_state.options.enemy_speed;
        enemy->velocity.y = direction_y * shooter_state.options.enemy_speed;

        move_object(state, *enemy, enemy->velocity.x * dt, enemy->velocity.y * dt, false);
    }

    void SimpleShooterBehaviour::update_bullets(SceneState& state, float dt)
    {
        for (auto& object : state.objects.objects()) {
            if (object.runtime.behaviour != simple_shooter_ids::bullet_behaviour || !object.active) {
                continue;
            }

            object.transform.x += object.velocity.x * dt;
            object.transform.y += object.velocity.y * dt;
            object.lifetime -= dt;

            if (object.lifetime <= 0.0f) {
                object.active = false;
            }
        }
    }

    void SimpleShooterBehaviour::handle_bullet_enemy_collisions(
        SceneState& state,
        const SimpleShooterState& shooter_state
    )
    {
        GameObject* enemy = enemy_object(state, shooter_state);
        if (!enemy || !enemy->active) {
            return;
        }

        for (auto& object : state.objects.objects()) {
            if (object.runtime.behaviour != simple_shooter_ids::bullet_behaviour || !object.active) {
                continue;
            }

            if (!collision::is_overlapping(object, *enemy)) {
                continue;
            }

            object.active = false;
            respawn_enemy(*enemy);
            return;
        }
    }

    void SimpleShooterBehaviour::respawn_enemy(GameObject& enemy) const noexcept
    {
        enemy.transform.x = 256.0f;
        enemy.transform.y = 128.0f;
        enemy.velocity = {};
        enemy.active = true;
        enemy.render.visible = true;
    }

    void SimpleShooterBehaviour::cleanup_runtime_objects(SceneState& state)
    {
        state.objects.remove_inactive_runtime_objects(simple_shooter_ids::bullet_behaviour);
    }
}
