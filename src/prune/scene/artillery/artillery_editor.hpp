#pragma once

#include "prune/scene/world_scene_editor.hpp"
#include "prune/tooling/artillery/artillery.hpp"

namespace prune {

  class ArtilleryEditor final : public WorldSceneEditor {
  public:
    void draw_scene_tools(Scene& scene, bool& open) override;
    void draw_scene_inspector(Scene& scene, GameObject& selected) override;

  private:
    Artillery m_artillery_tools;
  };

} // namespace prune
