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
  } // namespace

  void
  SceneSettingsEditTracker::capture_if_activated(const YAML::Node& before) {
    if (ImGui::IsItemActivated()) {
      m_before = before;
    }
  }

  void SceneSettingsEditTracker::commit_if_deactivated_after_edit(
      Scene& scene, std::string_view label, const YAML::Node& after,
      std::string_view detail) {
    if (!ImGui::IsItemDeactivatedAfterEdit()) {
      return;
    }

    if (!m_before.has_value()) {
      return;
    }

    const YAML::Node before = m_before.value();
    m_before.reset();

    if (yaml_nodes_equal(before, after)) {
      return;
    }

    scene.record_editor_command(
        make_scene_settings_change(label, before, after, detail));
  }

  void SceneSettingsEditTracker::reset() noexcept { m_before.reset(); }

} // namespace prune::tooling::editor::tracked_settings_table
