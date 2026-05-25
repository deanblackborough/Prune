#include "prune/scene/platformer/platformer_serializer.hpp"

namespace prune {

    void PlatformerSerializer::save_to_node(const PlatformerState& state, YAML::Node& root)
    {
        root["platformer"]["player_id"] = state.player_id;
        root["platformer"]["player_start_id"] = state.player_start_id;
        root["platformer"]["paused"] = state.options.paused;
        root["platformer"]["move_speed"] = state.options.move_speed;
        root["platformer"]["jump_velocity"] = state.options.jump_velocity;
        root["platformer"]["gravity"] = state.options.gravity;
        root["platformer"]["max_fall_speed"] = state.options.max_fall_speed;
    }

    bool PlatformerSerializer::load_from_node(const YAML::Node& root, PlatformerState& state, std::string& error)
    {
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

        if (platformer["paused"]) {
            state.options.paused = platformer["paused"].as<bool>();
        }

        if (platformer["move_speed"]) {
            state.options.move_speed = platformer["move_speed"].as<float>();
        }

        if (platformer["jump_velocity"]) {
            state.options.jump_velocity = platformer["jump_velocity"].as<float>();
        }

        if (platformer["gravity"]) {
            state.options.gravity = platformer["gravity"].as<float>();
        }

        if (platformer["max_fall_speed"]) {
            state.options.max_fall_speed = platformer["max_fall_speed"].as<float>();
        }

        return true;
    }

}
