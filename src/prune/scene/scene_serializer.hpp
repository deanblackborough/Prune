#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class SceneSerializer {
    public:
        static void save_to_node(const SceneState& state, const SceneCamera& camera, const GridOptions& grid_options, YAML::Node& root);

        [[nodiscard]] static bool load_from_node(
            SceneState& state,
            SceneCamera& camera,
            GridOptions& grid_options,
            const YAML::Node& root,
            std::string& error
        );
    };
}
