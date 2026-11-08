#pragma once

#include <optional>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include "prune/editor/editor_command.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {
  class Scene;
}

namespace prune::tooling::editor::tracked_settings_table {

  // Captures a full authored-settings YAML snapshot when a control becomes
  // active and, if the settings actually changed, commits a
  // SceneSettingsChange when it deactivates. One tracker instance is reused
  // across every control in a scene's settings panel, the same way
  // ObjectEditTracker is reused across a GameObject's fields.
  class SceneSettingsEditTracker {
  public:
    void capture_if_activated(const YAML::Node& before);
    void commit_if_deactivated_after_edit(Scene& scene, std::string_view label,
                                          const YAML::Node& after,
                                          std::string_view detail = {});
    void reset() noexcept;

  private:
    std::optional<YAML::Node> m_before;
  };

  template <typename Snapshot>
  bool slider_float(SceneSettingsEditTracker& tracker, Scene& scene,
                    Snapshot&& snapshot, std::string_view label,
                    const char* imgui_label, const char* id, float& value,
                    float min, float max, const char* format = "%.2f",
                    std::string_view detail = {}) {
    const YAML::Node before = snapshot();
    const bool changed = ::prune::tooling::imgui::property_table::slider_float(
        imgui_label, id, value, min, max, format);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, label, snapshot(), detail);

    return changed;
  }

  template <typename Snapshot>
  bool slider_int(SceneSettingsEditTracker& tracker, Scene& scene,
                  Snapshot&& snapshot, std::string_view label,
                  const char* imgui_label, const char* id, int& value, int min,
                  int max, std::string_view detail = {}) {
    const YAML::Node before = snapshot();
    const bool changed = ::prune::tooling::imgui::property_table::slider_int(
        imgui_label, id, value, min, max);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, label, snapshot(), detail);

    return changed;
  }

} // namespace prune::tooling::editor::tracked_settings_table
