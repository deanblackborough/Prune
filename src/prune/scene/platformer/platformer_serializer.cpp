#include "prune/scene/platformer/platformer_serializer.hpp"

namespace prune {

  void PlatformerSerializer::save_to_node(const PlatformerState& state,
                                          YAML::Node& root) {
    root["platformer"]["player_id"] = state.player_id;
    root["platformer"]["player_start_id"] = state.player_start_id;

    YAML::Node platformer = root["platformer"];
    save_settings_to_node(state.settings, platformer);
  }

  bool PlatformerSerializer::load_from_node(const YAML::Node& root,
                                            PlatformerState& state,
                                            std::string& error) {
    const YAML::Node platformer = root["platformer"];

    if (!platformer || !platformer.IsMap()) {
      error = "Save file is missing platformer section.";
      return false;
    }

    if (!platformer["player_id"]) {
      error = "platformer.player_id is missing.";
      return false;
    }

    state.player_id = platformer["player_id"].as<GameObjectId>();

    if (platformer["player_start_id"]) {
      state.player_start_id = platformer["player_start_id"].as<GameObjectId>();
    }

    load_settings_from_node(platformer, state.settings);

    return true;
  }

  void PlatformerSerializer::save_settings_to_node(
      const PlatformerSettings& settings, YAML::Node& node) {
    node["move_speed"] = settings.move_speed;
    node["jump_velocity"] = settings.jump_velocity;
    node["gravity"] = settings.gravity;
    node["max_fall_speed"] = settings.max_fall_speed;
  }

  void
  PlatformerSerializer::load_settings_from_node(const YAML::Node& node,
                                                PlatformerSettings& settings) {
    if (node["move_speed"]) {
      settings.move_speed = node["move_speed"].as<float>();
    }

    if (node["jump_velocity"]) {
      settings.jump_velocity = node["jump_velocity"].as<float>();
    }

    if (node["gravity"]) {
      settings.gravity = node["gravity"].as<float>();
    }

    if (node["max_fall_speed"]) {
      settings.max_fall_speed = node["max_fall_speed"].as<float>();
    }
  }

} // namespace prune
