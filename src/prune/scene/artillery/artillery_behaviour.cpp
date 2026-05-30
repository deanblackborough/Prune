#include <algorithm>
#include <array>
#include <cmath>
#include <random>
#include <string>
#include <vector>

#include <SDL2/SDL.h>

#include "prune/scene/artillery/artillery_behaviour.hpp"
#include "prune/scene/artillery/artillery_concepts.hpp"
#include "prune/scene/artillery/artillery_factory.hpp"
#include "prune/scene/artillery/artillery_ids.hpp"
#include "prune/scene/collision.hpp"

namespace prune {

    namespace {
        constexpr float k_degrees_to_radians = 3.1415926535f / 180.0f;

        struct TerrainSegment {
            float x = 0.0f;
            float y = 0.0f;
            int width = 0;
            int height = 0;
        };

        struct TerrainLayout {
            float player_one_x = 0.0f;
            float player_one_y = 0.0f;
            float player_two_x = 0.0f;
            float player_two_y = 0.0f;
            std::array<TerrainSegment, 5> segments{};
        };

        constexpr std::array<TerrainLayout, 3> k_terrain_layouts{ {
            {
                40.0f, 152.0f,
                328.0f, 128.0f,
                { {
                    { 0.0f, 184.0f, 80, 16 },
                    { 80.0f, 168.0f, 80, 16 },
                    { 160.0f, 152.0f, 80, 16 },
                    { 240.0f, 136.0f, 80, 16 },
                    { 320.0f, 160.0f, 96, 16 }
                } }
            },
            {
                40.0f, 136.0f,
                328.0f, 152.0f,
                { {
                    { 0.0f, 168.0f, 96, 16 },
                    { 96.0f, 152.0f, 64, 16 },
                    { 160.0f, 176.0f, 96, 16 },
                    { 256.0f, 160.0f, 64, 16 },
                    { 320.0f, 184.0f, 96, 16 }
                } }
            },
            {
                40.0f, 160.0f,
                328.0f, 144.0f,
                { {
                    { 0.0f, 192.0f, 96, 16 },
                    { 96.0f, 176.0f, 64, 16 },
                    { 160.0f, 160.0f, 96, 16 },
                    { 256.0f, 176.0f, 64, 16 },
                    { 320.0f, 176.0f, 96, 16 }
                } }
            }
        } };

        [[nodiscard]] bool is_active_blocking_artillery_object(const GameObject& object) noexcept
        {
            return object.lifecycle.active &&
                (artillery_concepts::is_tank(object) || artillery_concepts::is_terrain_line(object));
        }

        [[nodiscard]] const TerrainLayout& random_terrain_layout()
        {
            static std::mt19937 rng{ std::random_device{}() };
            std::uniform_int_distribution<std::size_t> distribution{ 0, k_terrain_layouts.size() - 1 };
            return k_terrain_layouts[distribution(rng)];
        }

        void remove_existing_terrain(SceneState& state)
        {
            std::vector<GameObjectId> terrain_ids;

            for (const auto& object : state.objects.objects()) {
                if (artillery_concepts::is_terrain_line(object)) {
                    terrain_ids.push_back(object.identity.id);
                }
            }

            for (const GameObjectId id : terrain_ids) {
                state.objects.remove_object(id);
            }
        }

        void apply_terrain_layout(SceneState& state, ArtilleryState& artillery_state, const TerrainLayout& layout)
        {
            remove_existing_terrain(state);

            if (GameObject* player_one = state.objects.get_by_id(artillery_state.player_one_id)) {
                player_one->lifecycle.active = true;
                player_one->transform.x = layout.player_one_x;
                player_one->transform.y = layout.player_one_y;
                player_one->motion.velocity = {};
            }

            if (GameObject* player_two = state.objects.get_by_id(artillery_state.player_two_id)) {
                player_two->lifecycle.active = true;
                player_two->transform.x = layout.player_two_x;
                player_two->transform.y = layout.player_two_y;
                player_two->motion.velocity = {};
            }

            int index = 1;
            for (const TerrainSegment& segment : layout.segments) {
                const std::string name = "Terrain Line " + std::to_string(index++);
                state.objects.create_object(artillery_factory::create_terrain_line(
                    segment.x,
                    segment.y,
                    segment.width,
                    segment.height,
                    name.c_str()
                ));
            }
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

        ArtilleryAim& aim = current_aim(artillery_state);

        if (input.is_key_down(SDL_SCANCODE_A) || input.is_key_down(SDL_SCANCODE_LEFT)) {
            aim.angle_degrees += artillery_state.options.angle_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_D) || input.is_key_down(SDL_SCANCODE_RIGHT)) {
            aim.angle_degrees -= artillery_state.options.angle_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_W) || input.is_key_down(SDL_SCANCODE_UP)) {
            aim.power += artillery_state.options.power_step * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_S) || input.is_key_down(SDL_SCANCODE_DOWN)) {
            aim.power -= artillery_state.options.power_step * dt;
        }

        aim.angle_degrees = std::clamp(aim.angle_degrees, 5.0f, 85.0f);
        aim.power = std::clamp(aim.power, artillery_state.options.min_power, artillery_state.options.max_power);

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
        const ArtilleryAim& aim = current_aim(artillery_state);
        const float radians = aim.angle_degrees * k_degrees_to_radians;
        const float direction = firing_right ? 1.0f : -1.0f;

        projectile.motion.velocity.x = std::cos(radians) * aim.power * direction;
        projectile.motion.velocity.y = -std::sin(radians) * aim.power;
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

    void ArtilleryBehaviour::reset_round(SceneState& state, ArtilleryState& artillery_state) const
    {
        if (GameObject* projectile = state.objects.get_by_id(artillery_state.projectile_id)) {
            projectile->lifecycle.active = false;
            state.objects.remove_inactive_runtime_objects(artillery_ids::projectile_behaviour);
        }

        apply_terrain_layout(state, artillery_state, random_terrain_layout());

        artillery_state.projectile_id = k_invalid_game_object_id;
        artillery_state.projectile_owner_id = k_invalid_game_object_id;
        artillery_state.projectile_active = false;
        artillery_state.current_turn = ArtilleryTurn::PlayerOne;
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
