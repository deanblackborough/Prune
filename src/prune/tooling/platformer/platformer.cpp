#include "imgui.h"

#include "prune/scene/platformer/platformer_serializer.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/platformer/platformer.hpp"

namespace prune {

  namespace {
    [[nodiscard]] YAML::Node
    settings_snapshot(const PlatformerSettings& settings) {
      YAML::Node node;
      PlatformerSerializer::save_settings_to_node(settings, node);
      return node;
    }
  } // namespace

  void Platformer::draw(Scene& scene, PlatformerState& state) {
    if (tooling::imgui::layout::collapsing_header("Game")) {
      if (tooling::imgui::property_table::begin("##platformer_game")) {
        tooling::imgui::property_table::text(
            "Grounded", state.runtime.player_grounded ? "Yes" : "No");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change move speed", "Move Speed", "##platformer_move_speed",
            state.settings.move_speed, 40.0f, 220.0f, "%.2f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change jump velocity", "Jump Velocity",
            "##platformer_jump_velocity", state.settings.jump_velocity, 120.0f,
            420.0f, "%.2f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change gravity", "Gravity", "##platformer_gravity",
            state.settings.gravity, 300.0f, 1400.0f, "%.2f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change max fall speed", "Max Fall Speed",
            "##platformer_max_fall_speed", state.settings.max_fall_speed,
            160.0f, 720.0f, "%.2f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change coyote time", "Coyote Time (s)", "##platformer_coyote_time",
            state.settings.coyote_time_seconds, 0.0f, 0.3f, "%.3f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change jump buffer", "Jump Buffer (s)", "##platformer_jump_buffer",
            state.settings.jump_buffer_seconds, 0.0f, 0.3f, "%.3f");

        tooling::imgui::property_table::end();
      }
    }

    if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
      ImGui::TextWrapped("One player, one player start marker, authored "
                         "platform objects, and authored hazard triggers.");
      ImGui::BulletText("A/D moves horizontally.");
      ImGui::BulletText("W or Space jumps only when grounded.");
      ImGui::BulletText("Hazards and falling out of the level reset the player "
                        "to Player Start.");
    }
  }

} // namespace prune
