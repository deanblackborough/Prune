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

    YAML::Node simple_shooter = root["simple_shooter"];
    save_settings_to_node(state.settings, simple_shooter);
  }

  bool SimpleShooterSerializer::load_from_node(const YAML::Node& root,
                                               SimpleShooterState& state,
                                               std::string& error) {
    const YAML::Node simple_shooter = root["simple_shooter"];

    if (!simple_shooter || !simple_shooter.IsMap()) {
      error = "Save file is missing simple_shooter section.";
      return false;
    }

    if (!simple_shooter["player_id"]) {
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

    return load_settings_from_node(simple_shooter, state.settings, error);
  }

  void SimpleShooterSerializer::save_settings_to_node(
      const SimpleShooterSettings& settings, YAML::Node& node) {
    node["player_speed"] = settings.player_speed;
    node["enemy_speed"] = settings.enemy_speed;
    node["projectile_speed"] = settings.projectile_speed;
    node["projectile_lifetime"] = settings.projectile_lifetime;
    node["fire_cooldown"] = settings.fire_cooldown;
    node["max_live_enemies"] = settings.max_live_enemies;
  }

  bool SimpleShooterSerializer::load_settings_from_node(
      const YAML::Node& node, SimpleShooterSettings& settings,
      std::string& error) {
    if (!node["player_speed"] || !node["enemy_speed"] ||
        (!has_projectile_options(node) && !has_legacy_bullet_options(node))) {
      error = "simple_shooter options are incomplete.";
      return false;
    }

    settings.player_speed = std::max(0.0f, node["player_speed"].as<float>());
    settings.enemy_speed = node["enemy_speed"].as<float>();

    if (has_projectile_options(node)) {
      settings.projectile_speed = node["projectile_speed"].as<float>();
      settings.projectile_lifetime = node["projectile_lifetime"].as<float>();
    } else {
      settings.projectile_speed = node["bullet_speed"].as<float>();
      settings.projectile_lifetime = node["bullet_lifetime"].as<float>();
    }

    if (node["fire_cooldown"]) {
      settings.fire_cooldown = node["fire_cooldown"].as<float>();
    }

    if (node["max_live_enemies"]) {
      settings.max_live_enemies = node["max_live_enemies"].as<int>();
    }

    return true;
  }
} // namespace prune
