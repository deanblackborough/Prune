#include "imgui.h"

#include "prune/tooling/artillery/artillery.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    namespace {
        [[nodiscard]] const char* turn_label(ArtilleryTurn turn) noexcept
        {
            return turn == ArtilleryTurn::PlayerTwo ? "Player Two" : "Player One";
        }
    }

    void Artillery::draw(ArtilleryState& state)
    {
        if (tooling::imgui::layout::collapsing_header("Game")) {
            if (tooling::imgui::property_table::begin("##artillery_game")) {
                tooling::imgui::property_table::checkbox("Paused", "##artillery_paused", state.options.paused);
                tooling::imgui::property_table::text("Current Player", turn_label(state.current_turn));
                tooling::imgui::property_table::text("Projectile Active", state.projectile_active ? "Yes" : "No");
                tooling::imgui::property_table::slider_float("Angle", "##artillery_angle", state.angle_degrees, 5.0f, 85.0f, "%.1f");
                tooling::imgui::property_table::slider_float("Power", "##artillery_power", state.power, state.options.min_power, state.options.max_power, "%.1f");
                tooling::imgui::property_table::slider_float("Gravity", "##artillery_gravity", state.options.gravity, 60.0f, 360.0f, "%.1f");
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
            ImGui::TextWrapped("Two shared controls drive the active tank. Firing creates a runtime projectile; terrain and tanks are authored scene objects.");
            ImGui::BulletText("A/D or Left/Right adjusts angle.");
            ImGui::BulletText("W/S or Up/Down adjusts power.");
            ImGui::BulletText("Space fires when no projectile is active.");
            ImGui::BulletText("R resets the round.");
            ImGui::BulletText("A tank hit resets the game immediately.");
        }
    }

}
