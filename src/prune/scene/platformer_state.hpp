#pragma once

#include "prune/scene/game_object.hpp"

namespace prune {

    struct PlatformerOptions {
        bool paused = false;
        float move_speed = 118.0f;
        float jump_velocity = 285.0f;
        float gravity = 860.0f;
        float max_fall_speed = 420.0f;
    };

    struct PlatformerState {
        GameObjectId player_id = k_invalid_game_object_id;
        GameObjectId player_start_id = k_invalid_game_object_id;
        PlatformerOptions options{};
        bool player_grounded = false;
    };

}
