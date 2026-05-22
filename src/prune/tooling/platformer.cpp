#include "imgui.h"

#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/platformer.hpp"

namespace prune {

    void Platformer::draw(PlatformerState& state)
    {
        if (tooling::imgui::layout::collapsing_header("Game")) {
            if (tooling::imgui::property_table::begin("##platformer_game")) {
                tooling::imgui::property_table::checkbox("Paused", "##platformer_paused", state.options.paused);
                tooling::imgui::property_table::text("Grounded", state.player_grounded ? "Yes" : "No");
                tooling::imgui::property_table::slider_float("Move Speed", "##platformer_move_speed", state.options.move_speed, 40.0f, 220.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Jump Velocity", "##platformer_jump_velocity", state.options.jump_velocity, 120.0f, 420.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Gravity", "##platformer_gravity", state.options.gravity, 300.0f, 1400.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Max Fall Speed", "##platformer_max_fall_speed", state.options.max_fall_speed, 160.0f, 720.0f, "%.2f");
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Slice Rules")) {
            ImGui::TextWrapped("One player, one player start marker, authored platform objects, and authored hazard triggers.");
            ImGui::BulletText("A/D moves horizontally.");
            ImGui::BulletText("W or Space jumps only when grounded.");
            ImGui::BulletText("Hazards and falling out of the level reset the player to Player Start.");
        }
    }

}
