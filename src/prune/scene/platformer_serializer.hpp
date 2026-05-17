#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer_state.hpp"

namespace prune {

    class PlatformerSerializer {
    public:
        static void save_to_node(const PlatformerState& state, YAML::Node& root);
        [[nodiscard]] static bool load_from_node(const YAML::Node& root, PlatformerState& state, std::string& error);
    };

}
