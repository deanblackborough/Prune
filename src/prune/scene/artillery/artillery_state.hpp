#pragma once

#include "prune/scene/game_object.hpp"

namespace prune {

    enum class ArtilleryTurn {
        PlayerOne = 0,
        PlayerTwo = 1
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

        ArtilleryTurn current_turn = ArtilleryTurn::PlayerOne;
        float angle_degrees = 45.0f;
        float power = 220.0f;
        bool projectile_active = false;

        ArtilleryOptions options{};
    };

}
