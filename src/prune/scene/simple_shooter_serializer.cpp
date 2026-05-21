#include "prune/scene/simple_shooter_serializer.hpp"

namespace prune {

    namespace {

        [[nodiscard]] bool has_projectile_options(const YAML::Node& simple_shooter)
        {
            return simple_shooter["projectile_speed"] && simple_shooter["projectile_lifetime"];
        }

        [[nodiscard]] bool has_legacy_bullet_options(const YAML::Node& simple_shooter)
        {
            return simple_shooter["bullet_speed"] && simple_shooter["bullet_lifetime"];
        }
    }

    void SimpleShooterSerializer::save_to_node(const SimpleShooterState& state, YAML::Node& root)
    {
        root["simple_shooter"]["player_id"] = state.player_id;
        root["simple_shooter"]["enemy_id"] = state.enemy_id;
        root["simple_shooter"]["enemy_spawn_id"] = state.enemy_spawn_id;
        root["simple_shooter"]["player_speed"] = state.player_controller.speed();

        root["simple_shooter"]["paused"] = state.options.paused;
        root["simple_shooter"]["enemy_speed"] = state.options.enemy_speed;
        root["simple_shooter"]["projectile_speed"] = state.options.projectile_speed;
        root["simple_shooter"]["projectile_lifetime"] = state.options.projectile_lifetime;
    }

    bool SimpleShooterSerializer::load_from_node(
        const YAML::Node& root,
        SimpleShooterState& state,
        std::string& error
    )
    {
        const YAML::Node simple_shooter = root["simple_shooter"];

        if (!simple_shooter || !simple_shooter.IsMap()) {
            error = "Save file is missing simple_shooter section.";
            return false;
        }

        if (!simple_shooter["player_id"] ||
            !simple_shooter["player_speed"] ||
            !simple_shooter["paused"] ||
            !simple_shooter["enemy_speed"] ||
            (!has_projectile_options(simple_shooter) && !has_legacy_bullet_options(simple_shooter))) {
            error = "simple_shooter options are incomplete.";
            return false;
        }

        state.player_id = simple_shooter["player_id"].as<GameObjectId>();
        state.enemy_id = simple_shooter["enemy_id"]
            ? simple_shooter["enemy_id"].as<GameObjectId>()
            : k_invalid_game_object_id;
        state.enemy_spawn_id = simple_shooter["enemy_spawn_id"]
            ? simple_shooter["enemy_spawn_id"].as<GameObjectId>()
            : k_invalid_game_object_id;
        state.player_controller.set_speed(simple_shooter["player_speed"].as<float>());

        state.options.paused = simple_shooter["paused"].as<bool>();
        state.options.enemy_speed = simple_shooter["enemy_speed"].as<float>();

        if (has_projectile_options(simple_shooter)) {
            state.options.projectile_speed = simple_shooter["projectile_speed"].as<float>();
            state.options.projectile_lifetime = simple_shooter["projectile_lifetime"].as<float>();
        }
        else {
            state.options.projectile_speed = simple_shooter["bullet_speed"].as<float>();
            state.options.projectile_lifetime = simple_shooter["bullet_lifetime"].as<float>();
        }

        return true;
    }
}
