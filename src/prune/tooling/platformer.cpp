#include "imgui.h"

#include "prune/tooling/platformer.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Platformer::draw(PlatformerOptions& options, bool grounded)
    {
        if (tooling::imgui::property_table::begin("##platformer_options")) {
            tooling::imgui::property_table::checkbox("Paused", "##paused", options.paused);
            tooling::imgui::property_table::text("Grounded", grounded ? "Yes" : "No");
            tooling::imgui::property_table::slider_float("Move speed", "##platformer_move_speed", options.move_speed, 0.0f, 256.0f, "%.2f");
            tooling::imgui::property_table::slider_float("Jump velocity", "##platformer_jump_velocity", options.jump_velocity, 0.0f, 512.0f, "%.2f");
            tooling::imgui::property_table::slider_float("Gravity", "##platformer_gravity", options.gravity, 0.0f, 1600.0f, "%.2f");
            tooling::imgui::property_table::slider_float("Max fall speed", "##platformer_max_fall_speed", options.max_fall_speed, 0.0f, 1024.0f, "%.2f");
            tooling::imgui::property_table::end();
        }
    }

}
