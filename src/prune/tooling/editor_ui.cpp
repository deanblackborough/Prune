#include "prune/tooling/editor_ui.hpp"

#include "imgui.h"
#include "prune/scene/scene.hpp"

namespace prune {

    void EditorUI::render(Scene& scene) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Scene Panel", nullptr, &m_show_scene_panel);
                ImGui::MenuItem("Objects Panel", nullptr, &m_show_object_panel);
                ImGui::MenuItem("Debug / Stats", nullptr, &m_show_debug);
                ImGui::MenuItem("ImGui Demo", nullptr, &m_show_imgui_demo);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (m_show_scene_panel) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 30.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300.0f, 350.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Scene Panel", &m_show_scene_panel)) {
                scene.draw_scene_panel();
            }
            ImGui::End();
        }

        if (m_show_object_panel) {
            ImGui::SetNextWindowPos(ImVec2(900.0f, 30.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(350.0f, 600.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Objects Panel", &m_show_object_panel)) {
                scene.draw_objects_panel();
            }
            ImGui::End();
        }

        if (m_show_debug) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 34.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(220.0f, 160.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Debug / Stats", &m_show_debug)) {
                ImGuiIO& io = ImGui::GetIO();

                ImGui::Text("FPS: %.1f", io.Framerate);
                ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);
                ImGui::Separator();

                scene.draw_debug_ui();
            }
            ImGui::End();
        }

        if (m_show_imgui_demo) {
            ImGui::ShowDemoWindow(&m_show_imgui_demo);
        }
    }

} // namespace prune