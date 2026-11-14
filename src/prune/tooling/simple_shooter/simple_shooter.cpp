#include "imgui.h"

#include "prune/scene/simple_shooter/simple_shooter_serializer.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/simple_shooter/simple_shooter.hpp"

namespace prune {

  namespace {
    [[nodiscard]] YAML::Node
    settings_snapshot(const SimpleShooterSettings& settings) {
      YAML::Node node;
      SimpleShooterSerializer::save_settings_to_node(settings, node);
      return node;
    }
  } // namespace

  void SimpleShooter::draw(Scene& scene, SimpleShooterState& state) {
    if (tooling::imgui::layout::collapsing_header("Game")) {
      if (tooling::imgui::property_table::begin("##simple_shooter_game")) {
        ImGui::BeginDisabled(!scene.runtime_stopped());

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change player speed", "Player Speed", "##player_speed",
            state.settings.player_speed, 0.0f, 512.0f, "%.2f");

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change enemy speed", "Enemy Speed", "##enemy_speed",
            state.settings.enemy_speed, 0.0f, 160.0f);

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change projectile speed", "Projectile Speed", "##projectile_speed",
            state.settings.projectile_speed, 40.0f, 400.0f);

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change projectile lifetime", "Projectile Lifetime",
            "##projectile_lifetime", state.settings.projectile_lifetime, 0.2f,
            4.0f);

        tooling::editor::tracked_settings_table::slider_float(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change fire cooldown", "Fire Cooldown", "##fire_cooldown",
            state.settings.fire_cooldown, 0.05f, 1.0f);

        tooling::editor::tracked_settings_table::slider_int(
            m_settings_tracker, scene,
            [&state] { return settings_snapshot(state.settings); },
            "Change max live enemies", "Max Live Enemies", "##max_live_enemies",
            state.settings.max_live_enemies, 0, 8);

        ImGui::EndDisabled();
        tooling::imgui::property_table::end();
      }
    }

    if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
      ImGui::TextWrapped("One player, one projectile type, one enemy type, "
                         "authored wall objects, and one enemy spawn marker.");
      ImGui::BulletText("Player movement is blocked by solid walls.");
      ImGui::BulletText("Projectiles hit enemies and stop at walls.");
      ImGui::BulletText(
          "Enemy/player contact has no damage rule in this slice.");
    }
  }
} // namespace prune
