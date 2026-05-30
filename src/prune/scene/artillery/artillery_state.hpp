#pragma once

#include "prune/scene/game_object.hpp"

namespace prune {

    enum class ArtilleryTurn {
        PlayerOne = 0,
        PlayerTwo = 1
    };

    struct ArtilleryAim {
        float angle_degrees = 45.0f;
        float power = 220.0f;
    };

    struct ArtilleryOptions {
        bool paused = false;
        float gravity = 180.0f;
        float projectile_lifetime = 8.0f;
        float min_power = 80.0f;
        float max_power = 340.0f;
        float angle_step = 45.0f;
        float power_step = 120.0f;
    };

    struct ArtilleryState {
        GameObjectId player_one_id = k_invalid_game_object_id;
        GameObjectId player_two_id = k_invalid_game_object_id;
        GameObjectId projectile_id = k_invalid_game_object_id;
        GameObjectId projectile_owner_id = k_invalid_game_object_id;

        ArtilleryTurn current_turn = ArtilleryTurn::PlayerOne;
        ArtilleryAim player_one_aim{};
        ArtilleryAim player_two_aim{};
        bool projectile_active = false;

        ArtilleryOptions options{};
    };

    [[nodiscard]] inline ArtilleryAim& current_aim(ArtilleryState& state) noexcept
    {
        return state.current_turn == ArtilleryTurn::PlayerOne
            ? state.player_one_aim
            : state.player_two_aim;
    }

    [[nodiscard]] inline const ArtilleryAim& current_aim(const ArtilleryState& state) noexcept
    {
        return state.current_turn == ArtilleryTurn::PlayerOne
            ? state.player_one_aim
            : state.player_two_aim;
    }

    [[nodiscard]] inline const ArtilleryAim& aim_for_turn(const ArtilleryState& state, ArtilleryTurn turn) noexcept
    {
        return turn == ArtilleryTurn::PlayerOne
            ? state.player_one_aim
            : state.player_two_aim;
    }

}
