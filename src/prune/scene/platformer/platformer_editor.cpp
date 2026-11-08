#include <cstdio>
#include <string>

#include "imgui.h"

#include "prune/scene/platformer/platformer_concepts.hpp"
#include "prune/scene/platformer/platformer_editor.hpp"
#include "prune/scene/platformer/platformer_scene.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  namespace {

    [[nodiscard]] const char* bool_label(bool value) noexcept {
      return value ? "Yes" : "No";
    }

  } // namespace

  void PlatformerEditor::draw_scene_tools(Scene& scene, bool& open) {
    auto& platformer_scene = static_cast<PlatformerScene&>(scene);

    tooling::EditorLayout::scene_panel();

    if (ImGui::Begin("Platformer", &open)) {
      draw_debug_tools(scene);

      ImGui::Separator();

      m_platformer_tools.draw(scene, platformer_scene.platformer_state());
    }

    ImGui::End();
  }

  void PlatformerEditor::draw_scene_inspector(Scene& scene,
                                              GameObject& selected) {
    const PlatformerState& state =
        static_cast<PlatformerScene&>(scene).platformer_state();

    if (!tooling::imgui::layout::collapsing_header("Platformer", true)) {
      return;
    }

    if (!tooling::imgui::property_table::begin("##platformer_inspector")) {
      return;
    }

    const auto kind = platformer_concepts::kind_for(selected);

    if (kind == platformer_concepts::ObjectKind::Player) {
      char velocity_buffer[64];
      std::snprintf(velocity_buffer, sizeof(velocity_buffer), "x %.1f, y %.1f",
                    selected.motion.velocity.x, selected.motion.velocity.y);

      tooling::imgui::property_table::text("Velocity", velocity_buffer);
      tooling::imgui::property_table::text(
          "Grounded", bool_label(state.runtime.player_grounded));
      tooling::imgui::property_table::text(
          "Move Speed",
          std::to_string(static_cast<int>(state.settings.move_speed))
              .c_str());
      tooling::imgui::property_table::text(
          "Jump Velocity",
          std::to_string(static_cast<int>(state.settings.jump_velocity))
              .c_str());
      tooling::imgui::property_table::text(
          "Gravity",
          std::to_string(static_cast<int>(state.settings.gravity)).c_str());
      tooling::imgui::property_table::text(
          "Reset Target Id", std::to_string(state.player_start_id).c_str());
    } else if (kind == platformer_concepts::ObjectKind::PlayerStart) {
      tooling::imgui::property_table::text(
          "Tracked Reset Marker",
          bool_label(selected.identity.id == state.player_start_id));
      tooling::imgui::property_table::text("Used By", "Hazard and fall reset");
      tooling::imgui::property_table::text(
          "Player Object Id", std::to_string(state.player_id).c_str());
    } else if (kind == platformer_concepts::ObjectKind::Platform) {
      tooling::imgui::property_table::text(
          "Collision Surface", bool_label(selected.collision.solid));
      tooling::imgui::property_table::text(
          "Can Ground Player", bool_label(selected.collision.solid));
    } else if (kind == platformer_concepts::ObjectKind::Hazard) {
      tooling::imgui::property_table::text("Resets Player", "On contact");
      tooling::imgui::property_table::text(
          "Reset Target Id", std::to_string(state.player_start_id).c_str());
    } else {
      tooling::imgui::property_table::text(
          "Platformer Behaviour", "No Platformer behaviour is assigned");
    }

    tooling::imgui::property_table::end();
  }

} // namespace prune
