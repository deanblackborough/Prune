#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/scene_state.hpp"

namespace prune {

  class SceneSerializer {
  public:
    static void save_to_node(const SceneState& state, YAML::Node& root);

    [[nodiscard]] static bool load_from_node(SceneState& state,
                                             const YAML::Node& root,
                                             std::string& error);
  };
} // namespace prune
