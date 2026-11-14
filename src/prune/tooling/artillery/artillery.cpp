#include "imgui.h"

#include "prune/scene/artillery/artillery_serializer.hpp"
#include "prune/tooling/artillery/artillery.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  namespace {
    [[nodiscard]] const char* turn_label(ArtilleryTurn turn) noexcept {
      return turn == ArtilleryTurn::PlayerTwo ? "Player Two" : "Player One";
    }

    [[nodiscard]] YAML::Node
    settings_snapshot(const ArtillerySettings& settings) {
      YAML::Node node;
      ArtillerySerializer::save_settings_to_node(settings, node);
      return node;
    }
  } // namespace

  void Artillery::draw(Scene& scene, ArtilleryState& state) {
    if (tooling::imgui::layout::collapsing_header("Game")) {
      if (tooling::imgui::property_table::begin("##artillery_game")) {
        ImGui::BeginDisabled(!scene.runtime_stopped());

        tooling::imgui::property_table::text(
            "Current Player", turn_label(state.runtime.current_turn));
        tooling::imgui::property_table::text(
            "Projectile Active",
            state.runtime.projectile_active ? "Yes" : "No");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change gravity", "Gravity", "##artillery_gravity",
            state.settings.gravity, 60.0f, 360.0f, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change projectile lifetime", "Projectile Lifetime (s)",
            "##artillery_projectile_lifetime",
            state.settings.projectile_lifetime, 1.0f, 20.0f, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change min power", "Min Power", "##artillery_min_power",
            state.settings.min_power, 0.0f, state.settings.max_power, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change max power", "Max Power", "##artillery_max_power",
            state.settings.max_power, state.settings.min_power, 800.0f, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change min angle", "Min Angle", "##artillery_min_angle",
            state.settings.min_angle_degrees, 0.0f,
            state.settings.max_angle_degrees, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change max angle", "Max Angle", "##artillery_max_angle",
            state.settings.max_angle_degrees, state.settings.min_angle_degrees,
            90.0f, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change angle step", "Angle Step (deg/s)", "##artillery_angle_step",
            state.settings.angle_step, 5.0f, 180.0f, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change power step", "Power Step", "##artillery_power_step",
            state.settings.power_step, 10.0f, 400.0f, "%.1f");

        ImGui::EndDisabled();
        tooling::imgui::property_table::end();
      }
    }

    if (tooling::imgui::layout::collapsing_header("Player Aims")) {
      if (tooling::imgui::property_table::begin("##artillery_player_aims")) {
        ImGui::BeginDisabled(!scene.runtime_stopped());

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change P1 angle", "P1 Angle", "##artillery_p1_angle",
            state.settings.initial_player_one_aim.angle_degrees,
            state.settings.min_angle_degrees, state.settings.max_angle_degrees,
            "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change P1 power", "P1 Power", "##artillery_p1_power",
            state.settings.initial_player_one_aim.power,
            state.settings.min_power, state.settings.max_power, "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change P2 angle", "P2 Angle", "##artillery_p2_angle",
            state.settings.initial_player_two_aim.angle_degrees,
            state.settings.min_angle_degrees, state.settings.max_angle_degrees,
            "%.1f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change P2 power", "P2 Power", "##artillery_p2_power",
            state.settings.initial_player_two_aim.power,
            state.settings.min_power, state.settings.max_power, "%.1f");

        ImGui::EndDisabled();
        tooling::imgui::property_table::end();
      }
    }

    if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
      ImGui::TextWrapped(
          "Two shared controls drive the active tank. Angle and power are "
          "stored per player. Firing creates a runtime projectile; terrain and "
          "tanks are authored scene objects.");
      ImGui::BulletText("A/D or Left/Right adjusts angle.");
      ImGui::BulletText("W/S or Up/Down adjusts power.");
      ImGui::BulletText("Space fires when no projectile is active.");
      ImGui::BulletText(
          "R resets the round with one of three fixed terrain layouts.");
      ImGui::BulletText("A tank hit resets the game immediately.");
    }
  }

} // namespace prune
