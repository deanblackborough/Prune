#include "imgui.h"

#include "prune/scene/scene.hpp"
#include "prune/tooling/editor/tracked_settings_table.hpp"

namespace prune::tooling::editor::tracked_settings_table {

  namespace {
    [[nodiscard]] bool yaml_nodes_equal(const YAML::Node& a,
                                        const YAML::Node& b) {
      YAML::Emitter emitter_a;
      emitter_a << a;
      YAML::Emitter emitter_b;
      emitter_b << b;

      return std::string_view(emitter_a.c_str()) ==
             std::string_view(emitter_b.c_str());
    }

    void commit_if_changed(Scene& scene, std::string_view label,
                           const YAML::Node& before, const YAML::Node& after,
                           std::string_view detail) {
      if (yaml_nodes_equal(before, after)) {
        return;
      }

      scene.record_editor_command(
          make_scene_settings_change(label, before, after, detail));
    }
  } // namespace

  void
  SceneSettingsEditTracker::capture_if_activated(const YAML::Node& before) {
    m_tracker.capture_if_activated(YAML::Clone(before));
  }

  void SceneSettingsEditTracker::commit_if_deactivated_after_edit(
      Scene& scene, std::string_view label, const YAML::Node& after,
      std::string_view detail) {
    const std::optional<YAML::Node> before =
        m_tracker.take_before_if_deactivated_after_edit();
    if (!before.has_value()) {
      return;
    }

    commit_if_changed(scene, label, *before, after, detail);
  }

  void SceneSettingsEditTracker::commit_pending_if_abandoned(
      Scene& scene, std::string_view label, const YAML::Node& current,
      std::string_view detail) {
    if (!ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
      return;
    }

    const std::optional<YAML::Node> before = m_tracker.take_pending();
    if (!before.has_value()) {
      return;
    }

    commit_if_changed(scene, label, *before, current, detail);
  }

  void SceneSettingsEditTracker::reset() noexcept { m_tracker.reset(); }

} // namespace prune::tooling::editor::tracked_settings_table
