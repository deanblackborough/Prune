#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/player_controller.hpp"

namespace prune {

    struct SimpleShooterOptions {
        bool paused = false;
        float enemy_speed = 24.0f;
        float projectile_speed = 180.0f;
        float projectile_lifetime = 1.25f;
    };

    struct SimpleShooterState {
        GameObjectId player_id = k_invalid_game_object_id;
        GameObjectId enemy_id = k_invalid_game_object_id;
        GameObjectId enemy_spawn_id = k_invalid_game_object_id;

        PlayerController player_controller{};
        SimpleShooterOptions options{};
    };
}
