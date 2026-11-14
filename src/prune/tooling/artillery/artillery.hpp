#pragma once

#include "prune/scene/artillery/artillery_state.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/editor/tracked_settings_table.hpp"

namespace prune {

  class Artillery {
  public:
    void draw(Scene& scene, ArtilleryState& state);

  private:
    tooling::editor::tracked_settings_table::SceneSettingsEditTracker
        m_settings_tracker;
  };

} // namespace prune
