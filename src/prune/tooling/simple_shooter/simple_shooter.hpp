#pragma once

#include "prune/scene/scene.hpp"
#include "prune/scene/simple_shooter/simple_shooter_state.hpp"
#include "prune/tooling/editor/tracked_settings_table.hpp"

namespace prune {

  class SimpleShooter {
  public:
    void draw(Scene& scene, SimpleShooterState& state);

  private:
    tooling::editor::tracked_settings_table::SceneSettingsEditTracker
        m_settings_tracker;
  };
} // namespace prune
