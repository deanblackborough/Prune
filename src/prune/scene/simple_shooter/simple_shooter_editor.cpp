#include <cstdio>
#include <string>

#include "imgui.h"

#include "prune/scene/simple_shooter/simple_shooter_concepts.hpp"
#include "prune/scene/simple_shooter/simple_shooter_editor.hpp"
#include "prune/scene/simple_shooter/simple_shooter_scene.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  namespace {

    [[nodiscard]] const char* bool_label(bool value) noexcept {
      return value ? "Yes" : "No";
    }

    [[nodiscard]] const char* direction_label(Direction direction) noexcept {
      switch (direction) {
      case Direction::Up:
        return "Up";
      case Direction::Down:
        return "Down";
      case Direction::Left:
        return "Left";
      case Direction::Right:
        return "Right";
      }

      return "Unknown";
    }

  } // namespace

  void SimpleShooterEditor::draw_scene_tools(Scene& scene, bool& open) {
    auto& shooter_scene = static_cast<SimpleShooterScene&>(scene);

    tooling::EditorLayout::scene_panel();

    if (ImGui::Begin("Simple Shooter", &open)) {
      draw_debug_tools(scene);

      ImGui::Separator();

      m_simple_shooter_tools.draw(scene, shooter_scene.simple_shooter_state());
    }

    ImGui::End();
  }

  void SimpleShooterEditor::draw_scene_inspector(Scene& scene,
                                                 GameObject& selected) {
    const SimpleShooterState& state =
        static_cast<SimpleShooterScene&>(scene).simple_shooter_state();

    if (!tooling::imgui::layout::collapsing_header("Simple Shooter", true)) {
      return;
    }

    if (!tooling::imgui::property_table::begin("##simple_shooter_inspector")) {
      return;
    }

    const auto kind = simple_shooter_concepts::kind_for(selected);

    if (kind == simple_shooter_concepts::ObjectKind::Player) {
      tooling::imgui::property_table::text(
          "Facing", direction_label(selected.motion.facing));
      tooling::imgui::property_table::text(
          "Controller Speed",
          std::to_string(static_cast<int>(state.settings.player_speed))
              .c_str());
      tooling::imgui::property_table::text(
          "Fire Ready",
          bool_label(state.runtime.fire_cooldown_remaining <= 0.0f));

      char cooldown_buffer[32];
      std::snprintf(cooldown_buffer, sizeof(cooldown_buffer), "%.2fs",
                    state.runtime.fire_cooldown_remaining);
      tooling::imgui::property_table::text("Cooldown Remaining",
                                           cooldown_buffer);
    } else if (kind == simple_shooter_concepts::ObjectKind::Enemy) {
      tooling::imgui::property_table::text(
          "Tracked Enemy",
          bool_label(selected.identity.id == state.enemy_id));
      tooling::imgui::property_table::text(
          "Enemy Speed",
          std::to_string(static_cast<int>(state.settings.enemy_speed))
              .c_str());
      tooling::imgui::property_table::text(
          "Spawn Object Id", std::to_string(state.enemy_spawn_id).c_str());
      tooling::imgui::property_table::text(
          "Max Live Enemies",
          std::to_string(state.settings.max_live_enemies).c_str());
    } else if (kind == simple_shooter_concepts::ObjectKind::Projectile) {
      char lifetime_buffer[32];
      std::snprintf(lifetime_buffer, sizeof(lifetime_buffer), "%.2fs",
                    selected.lifecycle.remaining);

      tooling::imgui::property_table::text("Remaining", lifetime_buffer);
      tooling::imgui::property_table::text(
          "Projectile Speed",
          std::to_string(static_cast<int>(state.settings.projectile_speed))
              .c_str());
      tooling::imgui::property_table::text(
          "Configured Lifetime",
          std::to_string(state.settings.projectile_lifetime).c_str());
    } else if (kind == simple_shooter_concepts::ObjectKind::Wall) {
      tooling::imgui::property_table::text(
          "Blocks Player", bool_label(selected.collision.solid));
      tooling::imgui::property_table::text(
          "Stops Projectiles", bool_label(selected.collision.solid));
    } else if (kind == simple_shooter_concepts::ObjectKind::Spawn) {
      tooling::imgui::property_table::text(
          "Tracked Spawn",
          bool_label(selected.identity.id == state.enemy_spawn_id));
      tooling::imgui::property_table::text("Used By",
                                           "Enemy respawn/reset loop");
      tooling::imgui::property_table::text(
          "Enemy Object Id", std::to_string(state.enemy_id).c_str());
    } else {
      tooling::imgui::property_table::text(
          "Shooter Behaviour", "No Simple Shooter behaviour is assigned");
    }

    tooling::imgui::property_table::end();
  }

} // namespace prune
