#include "imgui.h"

#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/simple_shooter/simple_shooter.hpp"

namespace prune {
    void SimpleShooter::draw(SimpleShooterState& state)
    {
        if (tooling::imgui::layout::collapsing_header("Game")) {
            if (tooling::imgui::property_table::begin("##simple_shooter_game")) {
                tooling::imgui::property_table::checkbox("Paused", "##paused", state.options.paused);

                float player_speed = state.player_controller.speed();

                if (tooling::imgui::property_table::slider_float(
                    "Player Speed",
                    "##player_speed",
                    player_speed,
                    0.0f,
                    512.0f,
                    "%.2f"
                )) {
                    state.player_controller.set_speed(player_speed);
                }

                tooling::imgui::property_table::slider_float("Enemy Speed", "##enemy_speed", state.options.enemy_speed, 0.0f, 160.0f);
                tooling::imgui::property_table::slider_float("Projectile Speed", "##projectile_speed", state.options.projectile_speed, 40.0f, 400.0f);
                tooling::imgui::property_table::slider_float("Projectile Lifetime", "##projectile_lifetime", state.options.projectile_lifetime, 0.2f, 4.0f);
                tooling::imgui::property_table::slider_float("Fire Cooldown", "##fire_cooldown", state.options.fire_cooldown, 0.05f, 1.0f);
                tooling::imgui::property_table::slider_int("Max Live Enemies", "##max_live_enemies", state.options.max_live_enemies, 0, 8);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
            ImGui::TextWrapped("One player, one projectile type, one enemy type, authored wall objects, and one enemy spawn marker.");
            ImGui::BulletText("Player movement is blocked by solid walls.");
            ImGui::BulletText("Projectiles hit enemies and stop at walls.");
            ImGui::BulletText("Enemy/player contact has no damage rule in this slice.");
        }
    }
}
