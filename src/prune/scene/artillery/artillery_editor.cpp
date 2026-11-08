#include <cstdio>
#include <string>

#include "imgui.h"

#include "prune/scene/artillery/artillery_concepts.hpp"
#include "prune/scene/artillery/artillery_editor.hpp"
#include "prune/scene/artillery/artillery_scene.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  namespace {

    [[nodiscard]] const char* bool_label(bool value) noexcept {
      return value ? "Yes" : "No";
    }

    [[nodiscard]] const char* turn_label(ArtilleryTurn turn) noexcept {
      return turn == ArtilleryTurn::PlayerTwo ? "Player Two" : "Player One";
    }

  } // namespace

  void ArtilleryEditor::draw_scene_tools(Scene& scene, bool& open) {
    auto& artillery_scene = static_cast<ArtilleryScene&>(scene);

    tooling::EditorLayout::scene_panel();

    if (ImGui::Begin("Artillery", &open)) {
      draw_debug_tools(scene);

      ImGui::Separator();

      m_artillery_tools.draw(scene, artillery_scene.artillery_state());
    }

    ImGui::End();
  }

  void ArtilleryEditor::draw_scene_inspector(Scene& scene,
                                             GameObject& selected) {
    const ArtilleryState& state =
        static_cast<ArtilleryScene&>(scene).artillery_state();

    if (!tooling::imgui::layout::collapsing_header("Artillery", true)) {
      return;
    }

    if (!tooling::imgui::property_table::begin("##artillery_inspector")) {
      return;
    }

    const auto kind = artillery_concepts::kind_for(selected);

    if (kind == artillery_concepts::ObjectKind::Tank) {
      tooling::imgui::property_table::text(
          "Current Player",
          bool_label(selected.identity.id ==
                     (state.runtime.current_turn == ArtilleryTurn::PlayerOne
                          ? state.player_one_id
                          : state.player_two_id)));
      tooling::imgui::property_table::text(
          "Turn", turn_label(state.runtime.current_turn));
      const ArtilleryAim& aim = selected.identity.id == state.player_two_id
                                    ? state.runtime.player_two_aim
                                    : state.runtime.player_one_aim;

      tooling::imgui::property_table::text(
          "Angle", std::to_string(static_cast<int>(aim.angle_degrees)).c_str());
      tooling::imgui::property_table::text(
          "Power", std::to_string(static_cast<int>(aim.power)).c_str());
    } else if (kind == artillery_concepts::ObjectKind::TerrainLine) {
      tooling::imgui::property_table::text("Generated Terrain", "Yes");
      tooling::imgui::property_table::text(
          "Projectile Collision", bool_label(selected.collision.solid));
    } else if (kind == artillery_concepts::ObjectKind::Projectile) {
      char velocity_buffer[64];
      std::snprintf(velocity_buffer, sizeof(velocity_buffer), "x %.1f, y %.1f",
                    selected.motion.velocity.x, selected.motion.velocity.y);
      tooling::imgui::property_table::text("Velocity", velocity_buffer);
      tooling::imgui::property_table::text("Runtime Object", "Yes");
    } else {
      tooling::imgui::property_table::text(
          "Artillery Behaviour", "No Artillery behaviour is assigned");
    }

    tooling::imgui::property_table::end();
  }

} // namespace prune
