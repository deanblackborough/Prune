#pragma once

#include "prune/scene/game_object.hpp"

namespace prune {

  struct PlatformerSettings {
    float move_speed = 118.0f;
    float jump_velocity = 285.0f;
    float gravity = 860.0f;
    float max_fall_speed = 420.0f;
  };

  struct PlatformerRuntime {
    bool paused = false;
    bool player_grounded = false;
  };

  struct PlatformerState {
    GameObjectId player_id = k_invalid_game_object_id;
    GameObjectId player_start_id = k_invalid_game_object_id;
    PlatformerSettings settings{};
    PlatformerRuntime runtime{};
  };

} // namespace prune
