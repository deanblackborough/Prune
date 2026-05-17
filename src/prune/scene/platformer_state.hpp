#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/player_controller.hpp"

namespace prune {

    struct PlatformerOptions {
        bool paused = false;
        float move_speed = 96.0f;
        float jump_velocity = 245.0f;
        float gravity = 720.0f;
        float max_fall_speed = 360.0f;
    };

    struct PlatformerState {
        GameObjectId player_id = k_invalid_game_object_id;
        PlatformerOptions options{};
        bool player_grounded = false;
    };

}
