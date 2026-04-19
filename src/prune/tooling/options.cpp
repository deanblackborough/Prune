#include "prune/tooling/options.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Options::draw(
        SceneOptions& scene_options,
        GridOptions& grid_options,
        CameraState& cameras
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
            if (tooling::imgui::property_table::begin("##camera_mode")) {
                int mode = (cameras.mode == CameraMode::Editor) ? 0 : 1;
                const char* mode_items[] = { "Editor", "Game" };

                if (tooling::imgui::property_table::combo("Active", "##camera_mode", mode, mode_items, IM_ARRAYSIZE(mode_items))) {
                    cameras.mode = (mode == 0) ? CameraMode::Editor : CameraMode::Game;
                }

                tooling::imgui::property_table::checkbox("Follow player", "##follow_player", cameras.game_options.follow_player);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Editor Camera")) {
            if (tooling::imgui::property_table::begin("##editor_camera")) {
                tooling::imgui::property_table::drag_float("X", "editor_x", cameras.editor.x, 4.0f);
                tooling::imgui::property_table::drag_float("Y", "editor_y", cameras.editor.y, 4.0f);
                tooling::imgui::property_table::slider_float("Speed", "editor_speed", cameras.editor.speed, 64.0f, 1024.0f);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Game Camera")) {
            if (tooling::imgui::property_table::begin("##game_camera")) {
                tooling::imgui::property_table::drag_float("X", "game_x", cameras.game.x, 4.0f);
                tooling::imgui::property_table::drag_float("Y", "game_y", cameras.game.y, 4.0f);
                tooling::imgui::property_table::slider_float("Speed", "game_speed", cameras.game.speed, 64.0f, 1024.0f);
                tooling::imgui::property_table::end();
            }
        }
    }
}
