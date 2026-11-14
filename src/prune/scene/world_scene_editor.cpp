#include "prune/scene/world_scene_editor.hpp"

#include <algorithm>

#include "imgui.h"

#include "prune/scene/scene.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  void WorldSceneEditor::draw_viewport_overlays(Scene& scene) {
    const SceneViewport& viewport = scene.get_viewport();
    if (!viewport.has_area()) {
      return;
    }

    if (!scene.get_scene_options().debug_overlays.show_role_labels) {
      return;
    }

    const WorldSceneContext context = scene.world_scene_context();
    if (!context.available()) {
      return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const Camera& camera = context.camera->active();
    const float zoom = std::max(camera.zoom, 0.01f);
    const ImU32 label_colour = IM_COL32(230, 220, 245, 230);
    const ImU32 label_shadow_colour = IM_COL32(18, 14, 24, 220);

    for (const auto& object : scene.get_object_manager().objects()) {
      if (!object.lifecycle.active || !object.render.visible) {
        continue;
      }

      const std::string label = scene.object_role_label(object);
      if (label.empty()) {
        continue;
      }

      const float screen_x = static_cast<float>(viewport.screen_x) +
                             ((object.transform.x - camera.x) * zoom);

      const float screen_y = static_cast<float>(viewport.screen_y) +
                             ((object.transform.y - camera.y) * zoom) - 18.0f;

      const ImVec2 pos{screen_x, screen_y};
      draw_list->AddText(ImVec2(pos.x + 1.0f, pos.y + 1.0f),
                         label_shadow_colour, label.c_str());
      draw_list->AddText(pos, label_colour, label.c_str());
    }
  }

  void WorldSceneEditor::draw_debug_tools(Scene& scene) {
    if (!tooling::imgui::layout::collapsing_header("Debug")) {
      return;
    }

    DebugOverlayOptions& overlays = scene.get_scene_options().debug_overlays;

    if (tooling::imgui::property_table::begin("Debug")) {
      tooling::imgui::property_table::checkbox("Collision bounds",
                                               "###collision_bounds",
                                               overlays.show_collision_bounds);
      tooling::imgui::property_table::checkbox("Runtime object markers",
                                               "###runtime_markers",
                                               overlays.show_runtime_markers);
      tooling::imgui::property_table::checkbox("Scene role labels",
                                               "###scene_role_labels",
                                               overlays.show_role_labels);
      tooling::imgui::property_table::end();
    }
  }

} // namespace prune
