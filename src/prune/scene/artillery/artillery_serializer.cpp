#include "prune/scene/artillery/artillery_serializer.hpp"

namespace prune {

  namespace {
    void save_aim(YAML::Node node, const ArtilleryAim& aim) {
      node["angle_degrees"] = aim.angle_degrees;
      node["power"] = aim.power;
    }

    [[nodiscard]] bool load_aim(const YAML::Node& node, ArtilleryAim& aim) {
      if (!node || !node.IsMap() || !node["angle_degrees"] || !node["power"]) {
        return false;
      }

      aim.angle_degrees = node["angle_degrees"].as<float>();
      aim.power = node["power"].as<float>();
      return true;
    }
  } // namespace

  void ArtillerySerializer::save_to_node(const ArtilleryState& state,
                                         YAML::Node& root) {
    YAML::Node artillery = root["artillery"];

    artillery["player_one_id"] = state.player_one_id;
    artillery["player_two_id"] = state.player_two_id;
    save_settings_to_node(state.settings, artillery);
  }

  bool ArtillerySerializer::load_from_node(const YAML::Node& root,
                                           ArtilleryState& state,
                                           std::string& error) {
    const YAML::Node artillery = root["artillery"];

    if (!artillery || !artillery.IsMap()) {
      error = "Save file is missing artillery section.";
      return false;
    }

    if (!artillery["player_one_id"] || !artillery["player_two_id"]) {
      error = "artillery options are incomplete.";
      return false;
    }

    state.player_one_id = artillery["player_one_id"].as<GameObjectId>();
    state.player_two_id = artillery["player_two_id"].as<GameObjectId>();

    if (!load_settings_from_node(artillery, state.settings, error)) {
      return false;
    }

    state.runtime = fresh_runtime(state.settings);

    return true;
  }

  void
  ArtillerySerializer::save_settings_to_node(const ArtillerySettings& settings,
                                             YAML::Node& node) {
    save_aim(node["initial_player_one_aim"], settings.initial_player_one_aim);
    save_aim(node["initial_player_two_aim"], settings.initial_player_two_aim);
    node["gravity"] = settings.gravity;
    node["projectile_lifetime"] = settings.projectile_lifetime;
    node["min_power"] = settings.min_power;
    node["max_power"] = settings.max_power;
    node["min_angle_degrees"] = settings.min_angle_degrees;
    node["max_angle_degrees"] = settings.max_angle_degrees;
    node["angle_step"] = settings.angle_step;
    node["power_step"] = settings.power_step;
  }

  bool ArtillerySerializer::load_settings_from_node(const YAML::Node& node,
                                                    ArtillerySettings& settings,
                                                    std::string& error) {
    const YAML::Node player_one_aim = node["initial_player_one_aim"]
                                          ? node["initial_player_one_aim"]
                                          : node["player_one_aim"];
    const YAML::Node player_two_aim = node["initial_player_two_aim"]
                                          ? node["initial_player_two_aim"]
                                          : node["player_two_aim"];

    if (!player_one_aim || !player_two_aim || !node["gravity"] ||
        !node["projectile_lifetime"] || !node["min_power"] ||
        !node["max_power"] || !node["angle_step"] || !node["power_step"]) {
      error = "artillery options are incomplete.";
      return false;
    }

    if (!load_aim(player_one_aim, settings.initial_player_one_aim) ||
        !load_aim(player_two_aim, settings.initial_player_two_aim)) {
      error = "artillery aim settings are incomplete.";
      return false;
    }

    settings.gravity = node["gravity"].as<float>();
    settings.projectile_lifetime = node["projectile_lifetime"].as<float>();
    settings.min_power = node["min_power"].as<float>();
    settings.max_power = node["max_power"].as<float>();
    settings.angle_step = node["angle_step"].as<float>();
    settings.power_step = node["power_step"].as<float>();

    if (node["min_angle_degrees"]) {
      settings.min_angle_degrees = node["min_angle_degrees"].as<float>();
    }

    if (node["max_angle_degrees"]) {
      settings.max_angle_degrees = node["max_angle_degrees"].as<float>();
    }

    return true;
  }

} // namespace prune
