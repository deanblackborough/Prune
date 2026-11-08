#pragma once

namespace prune {

  class Scene;
  struct GameObject;

  // The ImGui-owning presentation half of a scene, split out from the
  // runtime/authoring half (Scene/WorldScene and their subclasses) so the
  // latter can stay free of ImGui and tooling/ dependencies. One concrete
  // SceneEditor exists per scene type, constructed by SceneEditorFactory to
  // match the active Scene's type, and driven by the editor shell (Ui).
  class SceneEditor {
  public:
    virtual ~SceneEditor() = default;

    SceneEditor(const SceneEditor&) = delete;
    SceneEditor& operator=(const SceneEditor&) = delete;

    virtual void draw_scene_tools(Scene& scene, bool& open) = 0;
    virtual void draw_scene_inspector(Scene& scene, GameObject& selected) = 0;
    virtual void draw_viewport_overlays(Scene& scene) = 0;

  protected:
    SceneEditor() = default;
  };

} // namespace prune
