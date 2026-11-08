#pragma once

#include "prune/scene/game_object.hpp"

namespace prune {

  struct PlatformerSettings {
    float move_speed = 118.0f;
    float jump_velocity = 285.0f;
    float gravity = 860.0f;
    float max_fall_speed = 420.0f;

    // Seconds after leaving a platform where a jump still counts as
    // grounded.
    float coyote_time_seconds = 0.1f;

    // Seconds before landing where a jump press is remembered and fires
    // once grounded, instead of being dropped if pressed a frame early.
    float jump_buffer_seconds = 0.15f;
  };

  struct PlatformerRuntime {
    bool paused = false;
    bool player_grounded = false;

    // Coyote time: seconds left where a jump still counts as grounded after
    // leaving a platform. Jump buffer: seconds left where a jump pressed
    // just before landing still fires once grounded.
    float coyote_time_remaining = 0.0f;
    float jump_buffer_remaining = 0.0f;
  };

  struct PlatformerState {
    GameObjectId player_id = k_invalid_game_object_id;
    GameObjectId player_start_id = k_invalid_game_object_id;
    PlatformerSettings settings{};
    PlatformerRuntime runtime{};
  };

} // namespace prune
