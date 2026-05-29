#include "prune/scene/artillery/artillery_serializer.hpp"

namespace prune {

    namespace {
        [[nodiscard]] const char* turn_to_string(ArtilleryTurn turn) noexcept
        {
            return turn == ArtilleryTurn::PlayerTwo ? "player_two" : "player_one";
        }

        [[nodiscard]] ArtilleryTurn turn_from_string(const std::string& value) noexcept
        {
            return value == "player_two" ? ArtilleryTurn::PlayerTwo : ArtilleryTurn::PlayerOne;
        }
    }

    void ArtillerySerializer::save_to_node(const ArtilleryState& state, YAML::Node& root)
    {
        root["artillery"]["player_one_id"] = state.player_one_id;
        root["artillery"]["player_two_id"] = state.player_two_id;
        root["artillery"]["current_turn"] = turn_to_string(state.current_turn);
        root["artillery"]["angle_degrees"] = state.angle_degrees;
        root["artillery"]["power"] = state.power;
        root["artillery"]["paused"] = state.options.paused;
        root["artillery"]["gravity"] = state.options.gravity;
        root["artillery"]["projectile_lifetime"] = state.options.projectile_lifetime;
        root["artillery"]["min_power"] = state.options.min_power;
        root["artillery"]["max_power"] = state.options.max_power;
        root["artillery"]["angle_step"] = state.options.angle_step;
        root["artillery"]["power_step"] = state.options.power_step;
    }

    bool ArtillerySerializer::load_from_node(const YAML::Node& root, ArtilleryState& state, std::string& error)
    {
        const YAML::Node artillery = root["artillery"];

        if (!artillery || !artillery.IsMap()) {
            error = "Save file is missing artillery section.";
            return false;
        }

        if (!artillery["player_one_id"] || !artillery["player_two_id"] ||
            !artillery["current_turn"] || !artillery["angle_degrees"] ||
            !artillery["power"] || !artillery["paused"] || !artillery["gravity"] ||
            !artillery["projectile_lifetime"] || !artillery["min_power"] ||
            !artillery["max_power"] || !artillery["angle_step"] || !artillery["power_step"]) {
            error = "artillery options are incomplete.";
            return false;
        }

        state.player_one_id = artillery["player_one_id"].as<GameObjectId>();
        state.player_two_id = artillery["player_two_id"].as<GameObjectId>();
        state.current_turn = turn_from_string(artillery["current_turn"].as<std::string>());
        state.angle_degrees = artillery["angle_degrees"].as<float>();
        state.power = artillery["power"].as<float>();
        state.options.paused = artillery["paused"].as<bool>();
        state.options.gravity = artillery["gravity"].as<float>();
        state.options.projectile_lifetime = artillery["projectile_lifetime"].as<float>();
        state.options.min_power = artillery["min_power"].as<float>();
        state.options.max_power = artillery["max_power"].as<float>();
        state.options.angle_step = artillery["angle_step"].as<float>();
        state.options.power_step = artillery["power_step"].as<float>();
        state.projectile_id = k_invalid_game_object_id;
        state.projectile_owner_id = k_invalid_game_object_id;
        state.projectile_active = false;

        return true;
    }

}
