#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/simple_shooter/simple_shooter_state.hpp"

namespace prune {

  class SimpleShooterSerializer {
  public:
    static void save_to_node(const SimpleShooterState& state, YAML::Node& root);

    [[nodiscard]] static bool load_from_node(const YAML::Node& root,
                                             SimpleShooterState& state,
                                             std::string& error);

    static void save_settings_to_node(const SimpleShooterSettings& settings,
                                      YAML::Node& node);
    [[nodiscard]] static bool
    load_settings_from_node(const YAML::Node& node,
                            SimpleShooterSettings& settings,
                            std::string& error);
  };
} // namespace prune
