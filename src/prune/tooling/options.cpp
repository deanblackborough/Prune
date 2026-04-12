#include "imgui.h"

#include "prune/tooling/options.hpp"

namespace prune {

    void Options::draw(
        SceneOptions& scene_options,
        GridOptions& grid_options,
        Camera& camera
    )
    {
        if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Highlight selected", &scene_options.highlight_selected);
        }

        if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show", &grid_options.show_grid);
            ImGui::Checkbox("Snap to grid", &grid_options.snap_to_grid);
            ImGui::SliderInt("Grid size", &grid_options.grid_size, grid_options.min_grid_size, grid_options.max_grid_size);
            ImGui::SliderInt("Nudge step", &grid_options.nudge_step, grid_options.min_nudge_step, grid_options.max_nudge_step);
        }

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Camera X", &camera.x, -4096.0f, 4096.0f);
            ImGui::SliderFloat("Camera Y", &camera.y, -4096.0f, 4096.0f);
            ImGui::SliderFloat("Speed", &camera.speed, 64.0f, 512.0f);
        }
    }
}
