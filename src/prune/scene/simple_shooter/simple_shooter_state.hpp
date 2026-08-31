#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/simple_shooter/simple_shooter_player_controller.hpp"

namespace prune {

  struct SimpleShooterSettings {
    float player_speed = 96.0f;
    float enemy_speed = 24.0f;
    float projectile_speed = 180.0f;
    float projectile_lifetime = 1.25f;
    float fire_cooldown = 0.18f;
    int max_live_enemies = 1;
  };

  struct SimpleShooterRuntime {
    bool paused = false;
    float fire_cooldown_remaining = 0.0f;
  };

  struct SimpleShooterState {
    GameObjectId player_id = k_invalid_game_object_id;
    GameObjectId enemy_id = k_invalid_game_object_id;
    GameObjectId enemy_spawn_id = k_invalid_game_object_id;

    SimpleShooterPlayerController player_controller{};
    SimpleShooterSettings settings{};
    SimpleShooterRuntime runtime{};
  };
} // namespace prune
