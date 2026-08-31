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
    save_aim(artillery["initial_player_one_aim"],
             state.settings.initial_player_one_aim);
    save_aim(artillery["initial_player_two_aim"],
             state.settings.initial_player_two_aim);
    artillery["gravity"] = state.settings.gravity;
    artillery["projectile_lifetime"] = state.settings.projectile_lifetime;
    artillery["min_power"] = state.settings.min_power;
    artillery["max_power"] = state.settings.max_power;
    artillery["angle_step"] = state.settings.angle_step;
    artillery["power_step"] = state.settings.power_step;
  }

  bool ArtillerySerializer::load_from_node(const YAML::Node& root,
                                           ArtilleryState& state,
                                           std::string& error) {
    const YAML::Node artillery = root["artillery"];

    if (!artillery || !artillery.IsMap()) {
      error = "Save file is missing artillery section.";
      return false;
    }

    const YAML::Node player_one_aim = artillery["initial_player_one_aim"]
                                          ? artillery["initial_player_one_aim"]
                                          : artillery["player_one_aim"];
    const YAML::Node player_two_aim = artillery["initial_player_two_aim"]
                                          ? artillery["initial_player_two_aim"]
                                          : artillery["player_two_aim"];

    if (!artillery["player_one_id"] || !artillery["player_two_id"] ||
        !player_one_aim || !player_two_aim || !artillery["gravity"] ||
        !artillery["projectile_lifetime"] || !artillery["min_power"] ||
        !artillery["max_power"] || !artillery["angle_step"] ||
        !artillery["power_step"]) {
      error = "artillery options are incomplete.";
      return false;
    }

    state.player_one_id = artillery["player_one_id"].as<GameObjectId>();
    state.player_two_id = artillery["player_two_id"].as<GameObjectId>();

    if (!load_aim(player_one_aim, state.settings.initial_player_one_aim) ||
        !load_aim(player_two_aim, state.settings.initial_player_two_aim)) {
      error = "artillery aim settings are incomplete.";
      return false;
    }

    state.settings.gravity = artillery["gravity"].as<float>();
    state.settings.projectile_lifetime =
        artillery["projectile_lifetime"].as<float>();
    state.settings.min_power = artillery["min_power"].as<float>();
    state.settings.max_power = artillery["max_power"].as<float>();
    state.settings.angle_step = artillery["angle_step"].as<float>();
    state.settings.power_step = artillery["power_step"].as<float>();

    state.runtime = fresh_runtime(state.settings);

    return true;
  }

} // namespace prune
