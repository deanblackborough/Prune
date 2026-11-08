#pragma once

#include "prune/scene/world_scene_editor.hpp"
#include "prune/tooling/platformer/platformer.hpp"

namespace prune {

  class PlatformerEditor final : public WorldSceneEditor {
  public:
    void draw_scene_tools(Scene& scene, bool& open) override;
    void draw_scene_inspector(Scene& scene, GameObject& selected) override;

  private:
    Platformer m_platformer_tools;
  };

} // namespace prune
