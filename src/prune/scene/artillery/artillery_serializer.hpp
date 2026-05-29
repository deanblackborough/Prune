#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/artillery/artillery_state.hpp"

namespace prune {

    class ArtillerySerializer {
    public:
        static void save_to_node(const ArtilleryState& state, YAML::Node& root);
        [[nodiscard]] static bool load_from_node(const YAML::Node& root, ArtilleryState& state, std::string& error);
    };

}
