#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer/platformer_state.hpp"

namespace prune {

  class PlatformerSerializer {
  public:
    static void save_to_node(const PlatformerState& state, YAML::Node& root);
    [[nodiscard]] static bool load_from_node(const YAML::Node& root,
                                             PlatformerState& state,
                                             std::string& error);

    static void save_settings_to_node(const PlatformerSettings& settings,
                                      YAML::Node& node);
    static void load_settings_from_node(const YAML::Node& node,
                                        PlatformerSettings& settings);
  };

} // namespace prune
