#pragma once

#include "prune/scene/scene_editor.hpp"

namespace prune {

  class Scene;

  // Presentation shared by every WorldScene-based editor: the generic
  // role-label viewport overlay and the Debug checkbox panel. Both read and
  // write scene state only through Scene's public interface, so they need no
  // access to WorldScene's protected members. Scene-specific tools and
  // inspector content stay pure virtual here, for each concrete XEditor to
  // provide.
  class WorldSceneEditor : public SceneEditor {
  public:
    void draw_viewport_overlays(Scene& scene) override;

  protected:
    WorldSceneEditor() = default;

    void draw_debug_tools(Scene& scene);
  };

} // namespace prune
