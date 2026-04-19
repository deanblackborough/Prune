#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    void Ui::render(SandboxScene& scene) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {

                if (ImGui::MenuItem("Save Scene")) {
                    std::string error;
                    if (scene.save_to_file(kSceneFilePath, error)) {
                        m_file_status = "Saved scene to sandbox_scene.yml";
                        m_file_status_is_error = false;
                    }
                    else {
                        m_file_status = "Save failed: " + error;
                        m_file_status_is_error = true;
                    }
                }

                if (ImGui::MenuItem("Load Scene")) {
                    std::string error;
                    if (scene.load_from_file(kSceneFilePath, error)) {
                        m_file_status = "Loaded scene from sandbox_scene.yml";
                        m_file_status_is_error = false;
                    }
                    else {
                        m_file_status = "Load failed: " + error;
                        m_file_status_is_error = true;
                    }
                }

                ImGui::Separator();
                ImGui::MenuItem("Options", nullptr, &m_show_view_grid_options);

                if (!m_file_status.empty()) {
                    ImGui::Separator();
                    if (m_file_status_is_error) {
                        ImGui::TextWrapped("Status: %s", m_file_status.c_str());
                    }
                    else {
                        ImGui::TextWrapped("Status: %s", m_file_status.c_str());
                    }
                }

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
                    scene.get_grid_options(),
                    scene.get_camera()
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