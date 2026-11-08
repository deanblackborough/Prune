#pragma once

#include "prune/scene/world_scene_editor.hpp"
#include "prune/tooling/simple_shooter/simple_shooter.hpp"

namespace prune {

  class SimpleShooterEditor final : public WorldSceneEditor {
  public:
    void draw_scene_tools(Scene& scene, bool& open) override;
    void draw_scene_inspector(Scene& scene, GameObject& selected) override;

  private:
    SimpleShooter m_simple_shooter_tools;
  };

} // namespace prune
