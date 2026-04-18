#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    void Ui::render(SandboxScene& scene) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                ImGui::MenuItem("Options", nullptr, &m_show_view_grid_options);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Outliner", nullptr, &m_show_outliner);
                ImGui::MenuItem("Inspector", nullptr, &m_show_inspector);
                ImGui::MenuItem("Stats", nullptr, &m_show_stats);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Controls", nullptr, &m_show_controls);
                ImGui::MenuItem("ImGui Demo", nullptr, &m_show_imgui_demo);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (m_show_view_grid_options) {
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(
                viewport->GetCenter(),
                ImGuiCond_FirstUseEver,
                ImVec2(0.5f, 0.5f)
            );
            ImGui::SetNextWindowSize(ImVec2(320.0f, 375.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Options", &m_show_view_grid_options)) {
                m_options.draw(scene.get_scene_options(), scene.get_grid_options(), scene.get_camera());
            }
            ImGui::End();
        }

        if (m_show_outliner) {
            ImGui::SetNextWindowPos(ImVec2(960.0f, 23.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(310.0f, 230.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Outliner", &m_show_outliner)) {
                Camera& camera = scene.get_camera();
                GridOptions& grid_options = scene.get_grid_options();

                m_outliner.draw(
                    scene.get_object_manager(),
                    camera.x,
                    camera.y,
                    scene.get_viewport_width(),
                    scene.get_viewport_height(),
                    grid_options.snap_to_grid,
                    grid_options.grid_size
                );
            }
            ImGui::End();
        }

        if (m_show_inspector) {
            ImGui::SetNextWindowPos(ImVec2(960.0f, 260.f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(310.0f, 460.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Inspector", &m_show_inspector)) {
                m_inspector.draw(
                    scene.get_object_manager(),
                    scene.get_player_id(),
                    scene.get_player_controller(),
                    scene.get_grid_options()
                );
            }
            ImGui::End();
        }

        if (m_show_controls) {

            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(
                viewport->GetCenter(),
                ImGuiCond_FirstUseEver,
                ImVec2(0.5f, 0.5f)
            );
            ImGui::SetNextWindowSize(ImVec2(377.0f, 165.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Controls", &m_show_controls)) {
                m_controls.draw();
            }
            ImGui::End();
        }

        if (m_show_stats) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 34.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(250.0f, 240.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Stats", &m_show_stats)) {
                m_stats.draw(scene.get_object_manager(), scene.get_player_id(), scene.get_viewport_width(), scene.get_viewport_height());
            }
            ImGui::End();
        }

        if (m_show_imgui_demo) {
            ImGui::ShowDemoWindow(&m_show_imgui_demo);
        }
    }
} 