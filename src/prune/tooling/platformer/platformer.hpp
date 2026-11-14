#pragma once

#include "prune/scene/platformer/platformer_state.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/editor/tracked_settings_table.hpp"

namespace prune {

  class Platformer {
  public:
    void draw(Scene& scene, PlatformerState& state);

  private:
    tooling::editor::tracked_settings_table::SceneSettingsEditTracker
        m_settings_tracker;
  };

} // namespace prune
