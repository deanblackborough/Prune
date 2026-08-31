#include "prune/scene/simple_shooter/simple_shooter_serializer.hpp"

#include <algorithm>

namespace prune {

  namespace {

    [[nodiscard]] bool
    has_projectile_options(const YAML::Node& simple_shooter) {
      return simple_shooter["projectile_speed"] &&
             simple_shooter["projectile_lifetime"];
    }

    [[nodiscard]] bool
    has_legacy_bullet_options(const YAML::Node& simple_shooter) {
      return simple_shooter["bullet_speed"] &&
             simple_shooter["bullet_lifetime"];
    }
  } // namespace

  void SimpleShooterSerializer::save_to_node(const SimpleShooterState& state,
                                             YAML::Node& root) {
    root["simple_shooter"]["player_id"] = state.player_id;
    root["simple_shooter"]["enemy_id"] = state.enemy_id;
    root["simple_shooter"]["enemy_spawn_id"] = state.enemy_spawn_id;
    root["simple_shooter"]["player_speed"] = state.settings.player_speed;

    root["simple_shooter"]["enemy_speed"] = state.settings.enemy_speed;
    root["simple_shooter"]["projectile_speed"] =
        state.settings.projectile_speed;
    root["simple_shooter"]["projectile_lifetime"] =
        state.settings.projectile_lifetime;
    root["simple_shooter"]["fire_cooldown"] = state.settings.fire_cooldown;
    root["simple_shooter"]["max_live_enemies"] =
        state.settings.max_live_enemies;
  }

  bool SimpleShooterSerializer::load_from_node(const YAML::Node& root,
                                               SimpleShooterState& state,
                                               std::string& error) {
    const YAML::Node simple_shooter = root["simple_shooter"];

    if (!simple_shooter || !simple_shooter.IsMap()) {
      error = "Save file is missing simple_shooter section.";
      return false;
    }

    if (!simple_shooter["player_id"] || !simple_shooter["player_speed"] ||
        !simple_shooter["enemy_speed"] ||
        (!has_projectile_options(simple_shooter) &&
         !has_legacy_bullet_options(simple_shooter))) {
      error = "simple_shooter options are incomplete.";
      return false;
    }

    state.player_id = simple_shooter["player_id"].as<GameObjectId>();
    state.enemy_id = simple_shooter["enemy_id"]
                         ? simple_shooter["enemy_id"].as<GameObjectId>()
                         : k_invalid_game_object_id;
    state.enemy_spawn_id =
        simple_shooter["enemy_spawn_id"]
            ? simple_shooter["enemy_spawn_id"].as<GameObjectId>()
            : k_invalid_game_object_id;
    state.settings.player_speed =
        std::max(0.0f, simple_shooter["player_speed"].as<float>());

    state.settings.enemy_speed = simple_shooter["enemy_speed"].as<float>();

    if (has_projectile_options(simple_shooter)) {
      state.settings.projectile_speed =
          simple_shooter["projectile_speed"].as<float>();
      state.settings.projectile_lifetime =
          simple_shooter["projectile_lifetime"].as<float>();
    } else {
      state.settings.projectile_speed =
          simple_shooter["bullet_speed"].as<float>();
      state.settings.projectile_lifetime =
          simple_shooter["bullet_lifetime"].as<float>();
    }

    if (simple_shooter["fire_cooldown"]) {
      state.settings.fire_cooldown =
          simple_shooter["fire_cooldown"].as<float>();
    }

    if (simple_shooter["max_live_enemies"]) {
      state.settings.max_live_enemies =
          simple_shooter["max_live_enemies"].as<int>();
    }

    return true;
  }
} // namespace prune
