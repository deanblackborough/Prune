#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>

#include "prune/scene/artillery/artillery_behaviour.hpp"
#include "prune/scene/artillery/artillery_concepts.hpp"
#include "prune/scene/artillery/artillery_factory.hpp"
#include "prune/scene/artillery/artillery_ids.hpp"
#include "prune/scene/collision.hpp"

namespace prune {

    namespace {
        constexpr float k_degrees_to_radians = 3.1415926535f / 180.0f;

        [[nodiscard]] bool is_active_blocking_artillery_object(const GameObject& object) noexcept
        {
            return object.lifecycle.active &&
                (artillery_concepts::is_tank(object) || artillery_concepts::is_terrain_line(object));
        }
    }

    void ArtilleryBehaviour::update(
        SceneState& state,
        SceneCamera&,
        ArtilleryState& artillery_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    ) const
    {
        if (artillery_state.options.paused) {
            return;
        }

        update_controls(state, artillery_state, dt, input, keyboard_input_enabled);
        update_projectile(state, artillery_state, dt);
    }

    void ArtilleryBehaviour::update_controls(
        SceneState& state,
        ArtilleryState& artillery_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    ) const
    {
        if (!keyboard_input_enabled || artillery_state.projectile_active) {
            return;
        }

        if (input.is_key_down(SDL_SCANCODE_A) || input.is_key_down(SDL_SCANCODE_LEFT)) {
            artillery_state.angle_degrees += artillery_state.options.angle_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_D) || input.is_key_down(SDL_SCANCODE_RIGHT)) {
            artillery_state.angle_degrees -= artillery_state.options.angle_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_W) || input.is_key_down(SDL_SCANCODE_UP)) {
            artillery_state.power += artillery_state.options.power_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_S) || input.is_key_down(SDL_SCANCODE_DOWN)) {
            artillery_state.power -= artillery_state.options.power_step * dt;
        }

        artillery_state.angle_degrees = std::clamp(artillery_state.angle_degrees, 5.0f, 85.0f);
        artillery_state.power = std::clamp(artillery_state.power, artillery_state.options.min_power, artillery_state.options.max_power);

        if (input.was_key_pressed(SDL_SCANCODE_SPACE)) {
            fire_projectile(state, artillery_state);
        }

        if (input.was_key_pressed(SDL_SCANCODE_R)) {
            reset_round(state, artillery_state);
        }
    }

    void ArtilleryBehaviour::fire_projectile(SceneState& state, ArtilleryState& artillery_state) const
    {
        const GameObject* tank = current_tank(state, artillery_state);
        if (!tank) {
            return;
        }

        const bool firing_right = artillery_state.current_turn == ArtilleryTurn::PlayerOne;
        GameObject projectile = artillery_factory::create_projectile(0.0f, 0.0f);
        const float projectile_width = static_cast<float>(projectile.size.width);
        const float muzzle_x = firing_right
            ? tank->transform.x + static_cast<float>(tank->size.width) + 2.0f
            : tank->transform.x - projectile_width - 2.0f;
        const float muzzle_y = tank->transform.y + 3.0f;

        projectile.transform.x = muzzle_x;
        projectile.transform.y = muzzle_y;
        const float radians = artillery_state.angle_degrees * k_degrees_to_radians;
        const float direction = firing_right ? 1.0f : -1.0f;

        projectile.motion.velocity.x = std::cos(radians) * artillery_state.power * direction;
        projectile.motion.velocity.y = -std::sin(radians) * artillery_state.power;
        projectile.lifecycle.remaining = artillery_state.options.projectile_lifetime;

        artillery_state.projectile_id = state.objects.create_object(projectile);
        artillery_state.projectile_owner_id = tank->identity.id;
        artillery_state.projectile_active = true;
    }

    void ArtilleryBehaviour::update_projectile(SceneState& state, ArtilleryState& artillery_state, float dt) const
    {
        if (!artillery_state.projectile_active) {
            return;
        }

        GameObject* projectile = state.objects.get_by_id(artillery_state.projectile_id);
        if (!projectile || !artillery_concepts::is_projectile(*projectile)) {
            artillery_state.projectile_id = k_invalid_game_object_id;
            artillery_state.projectile_owner_id = k_invalid_game_object_id;
            artillery_state.projectile_active = false;
            return;
        }

        projectile->motion.velocity.y += artillery_state.options.gravity * dt;
        projectile->transform.x += projectile->motion.velocity.x * dt;
        projectile->transform.y += projectile->motion.velocity.y * dt;
        projectile->lifecycle.remaining -= dt;

        GameObjectId hit_id = k_invalid_game_object_id;
        if (projectile->lifecycle.remaining <= 0.0f || projectile->transform.y > 360.0f || projectile_hit_solid(state, *projectile, artillery_state.projectile_owner_id, hit_id)) {
            if (GameObject* hit = state.objects.get_by_id(hit_id); hit && artillery_concepts::is_tank(*hit)) {
                reset_round(state, artillery_state);
                return;
            }

            projectile->lifecycle.active = false;
            state.objects.remove_inactive_runtime_objects(artillery_ids::projectile_behaviour);
            artillery_state.projectile_id = k_invalid_game_object_id;
            artillery_state.projectile_owner_id = k_invalid_game_object_id;
            artillery_state.projectile_active = false;
            end_turn(state, artillery_state);
        }
    }

    bool ArtilleryBehaviour::projectile_hit_solid(const SceneState& state, const GameObject& projectile, GameObjectId ignored_object_id, GameObjectId& hit_id) const noexcept
    {
        for (const auto& object : state.objects.objects()) {
            if (object.identity.id == projectile.identity.id ||
                object.identity.id == ignored_object_id ||
                !is_active_blocking_artillery_object(object)) {
                continue;
            }

            if (collision::are_active_overlapping(projectile, object)) {
                hit_id = object.identity.id;
                return true;
            }
        }

        return false;
    }

    void ArtilleryBehaviour::end_turn(SceneState&, ArtilleryState& artillery_state) const noexcept
    {
        artillery_state.current_turn = artillery_state.current_turn == ArtilleryTurn::PlayerOne
            ? ArtilleryTurn::PlayerTwo
            : ArtilleryTurn::PlayerOne;
    }

    void ArtilleryBehaviour::reset_round(SceneState& state, ArtilleryState& artillery_state) const noexcept
    {
        if (GameObject* projectile = state.objects.get_by_id(artillery_state.projectile_id)) {
            projectile->lifecycle.active = false;
            state.objects.remove_inactive_runtime_objects(artillery_ids::projectile_behaviour);
        }

        if (GameObject* player_one = state.objects.get_by_id(artillery_state.player_one_id)) {
            player_one->lifecycle.active = true;
            player_one->transform.x = 40.0f;
            player_one->transform.y = 152.0f;
            player_one->motion.velocity = {};
        }

        if (GameObject* player_two = state.objects.get_by_id(artillery_state.player_two_id)) {
            player_two->lifecycle.active = true;
            player_two->transform.x = 328.0f;
            player_two->transform.y = 128.0f;
            player_two->motion.velocity = {};
        }

        artillery_state.projectile_id = k_invalid_game_object_id;
        artillery_state.projectile_owner_id = k_invalid_game_object_id;
        artillery_state.projectile_active = false;
        artillery_state.current_turn = ArtilleryTurn::PlayerOne;
        artillery_state.angle_degrees = 45.0f;
        artillery_state.power = 220.0f;
    }

    GameObject* ArtilleryBehaviour::current_tank(SceneState& state, const ArtilleryState& artillery_state) const noexcept
    {
        const GameObjectId id = artillery_state.current_turn == ArtilleryTurn::PlayerOne
            ? artillery_state.player_one_id
            : artillery_state.player_two_id;

        GameObject* object = state.objects.get_by_id(id);
        return object && artillery_concepts::is_tank(*object) ? object : nullptr;
    }

    const GameObject* ArtilleryBehaviour::current_tank(const SceneState& state, const ArtilleryState& artillery_state) const noexcept
    {
        const GameObjectId id = artillery_state.current_turn == ArtilleryTurn::PlayerOne
            ? artillery_state.player_one_id
            : artillery_state.player_two_id;

        const GameObject* object = state.objects.get_by_id(id);
        return object && artillery_concepts::is_tank(*object) ? object : nullptr;
    }

}
