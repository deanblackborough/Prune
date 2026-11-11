#pragma once

#include "prune/scene/editor_selection.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

  class Stats {
  public:
    void draw(const GameObjectManager& objects,
              const EditorSelection& selection, const SceneViewport& viewport,
              const SceneCamera& camera);

  private:
  };
} // namespace prune
