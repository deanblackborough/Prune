#include <algorithm>

#include <SDL2/SDL.h>

#include "prune/scene/collision.hpp"
#include "prune/scene/platformer_behaviour.hpp"
#include "prune/scene/platformer_ids.hpp"

namespace prune {

    void PlatformerBehaviour::update(
        SceneState& state,
        PlatformerState& platformer_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    )
    {
        if (platformer_state.options.paused) {
            return;
        }

        update_player(state, platformer_state, dt, input, keyboard_input_enabled);
    }

    GameObject* PlatformerBehaviour::player_object(SceneState& state, const PlatformerState& platformer_state) const noexcept
    {
        return state.objects.get_by_id(platformer_state.player_id);
    }

    const GameObject* PlatformerBehaviour::player_object(const SceneState& state, const PlatformerState& platformer_state) const noexcept
    {
        return state.objects.get_by_id(platformer_state.player_id);
    }

    void PlatformerBehaviour::update_player(
        SceneState& state,
        PlatformerState& platformer_state,
        float dt,
        const Input& input,
        bool keyboard_input_enabled
    ) const
    {
        GameObject* player = player_object(state, platformer_state);
        if (!player) {
            return;
        }

        float horizontal = 0.0f;

        if (keyboard_input_enabled) {
            if (input.is_key_down(SDL_SCANCODE_A)) {
                horizontal -= 1.0f;
            }

            if (input.is_key_down(SDL_SCANCODE_D)) {
                horizontal += 1.0f;
            }

            if (horizontal < 0.0f) {
                player->motion.facing = Direction::Left;
            }
            else if (horizontal > 0.0f) {
                player->motion.facing = Direction::Right;
            }

            if ((input.was_key_pressed(SDL_SCANCODE_W) || input.was_key_pressed(SDL_SCANCODE_SPACE)) && platformer_state.player_grounded) {
                player->motion.velocity.y = -platformer_state.options.jump_velocity;
                platformer_state.player_grounded = false;
            }
        }

        player->motion.velocity.x = horizontal * platformer_state.options.move_speed;
        player->motion.velocity.y = std::min(
            player->motion.velocity.y + (platformer_state.options.gravity * dt),
            platformer_state.options.max_fall_speed
        );

        bool grounded = false;

        move_player_axis(state, *player, player->motion.velocity.x * dt, 0.0f, grounded);
        move_player_axis(state, *player, 0.0f, player->motion.velocity.y * dt, grounded);

        platformer_state.player_grounded = grounded;

        if (grounded && player->motion.velocity.y > 0.0f) {
            player->motion.velocity.y = 0.0f;
        }

        if (touches_hazard(state, *player) || player->transform.y > 512.0f) {
            reset_player(state, platformer_state);
        }

        if (horizontal != 0.0f || player->motion.velocity.y != 0.0f) {
            state.camera.activate_game();
        }
    }

    void PlatformerBehaviour::move_player_axis(SceneState& state, GameObject& player, float delta_x, float delta_y, bool& grounded) const
    {
        player.transform.x += delta_x;
        player.transform.y += delta_y;

        for (const auto& object : state.objects.objects()) {
            if (object.identity.id == player.identity.id || !object.lifecycle.active || !object.collision.solid) {
                continue;
            }

            if (!collision::is_overlapping(player, object)) {
                continue;
            }

            if (delta_x > 0.0f) {
                player.transform.x = object.transform.x - static_cast<float>(player.size.width);
                player.motion.velocity.x = 0.0f;
            }
            else if (delta_x < 0.0f) {
                player.transform.x = object.transform.x + static_cast<float>(object.size.width);
                player.motion.velocity.x = 0.0f;
            }

            if (delta_y > 0.0f) {
                player.transform.y = object.transform.y - static_cast<float>(player.size.height);
                player.motion.velocity.y = 0.0f;
                grounded = true;
            }
            else if (delta_y < 0.0f) {
                player.transform.y = object.transform.y + static_cast<float>(object.size.height);
                player.motion.velocity.y = 0.0f;
            }
        }
    }

    bool PlatformerBehaviour::touches_hazard(const SceneState& state, const GameObject& player) const noexcept
    {
        for (const auto& object : state.objects.objects()) {
            if (object.runtime.behaviour != platformer_ids::hazard_behaviour || !object.lifecycle.active) {
                continue;
            }

            if (collision::is_overlapping(player, object)) {
                return true;
            }
        }

        return false;
    }

    void PlatformerBehaviour::reset_player(SceneState& state, PlatformerState& platformer_state) const noexcept
    {
        GameObject* player = player_object(state, platformer_state);
        if (!player) {
            return;
        }

        player->transform.x = 64.0f;
        player->transform.y = 96.0f;
        player->motion.velocity = {};
        player->motion.facing = Direction::Right;
        platformer_state.player_grounded = false;
    }

}
