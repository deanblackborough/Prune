#include "prune/tooling/options.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Options::draw(
        SceneOptions& scene_options,
        GridOptions& grid_options,
        Camera& camera
    )
    {
		if (tooling::imgui::layout::collapsing_header("Scene")) {
			tooling::imgui::property_table::begin("##scene");
			tooling::imgui::property_table::checkbox("Highlight selected", "##highlight_selected", scene_options.highlight_selected);
			tooling::imgui::property_table::end();
        }

        if (tooling::imgui::layout::collapsing_header("Grid")) {
            tooling::imgui::property_table::begin("##grid");
            tooling::imgui::property_table::checkbox("Show", "##show", grid_options.show_grid);
            tooling::imgui::property_table::checkbox("Snap to grid", "##snap_to_grid", grid_options.snap_to_grid);
            tooling::imgui::property_table::slider_int("Grid size", "##grid_size", grid_options.grid_size, grid_options.min_grid_size, grid_options.max_grid_size);
            tooling::imgui::property_table::slider_int("Nudge step", "##nudge_step", grid_options.nudge_step, grid_options.min_nudge_step, grid_options.max_nudge_step);
            tooling::imgui::property_table::end();
        }

        if (tooling::imgui::layout::collapsing_header("Camera")) {
            tooling::imgui::property_table::begin("##camera");
            tooling::imgui::property_table::slider_float("X", "##x", camera.x, -4096.0f, 4096.0f);
            tooling::imgui::property_table::slider_float("Y", "##y", camera.y, -4096.0f, 4096.0f);
            tooling::imgui::property_table::slider_float("Speed", "##speed", camera.speed, 64.0f, 512.0f);
            tooling::imgui::property_table::end();
        }
    }
}
