#include "prune/tooling/editor_ui.hpp"

#include "imgui.h"
#include "prune/core/scene.hpp"

namespace prune {

    void EditorUI::render(Scene& scene) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Inspector", nullptr, &m_show_inspector);
                ImGui::MenuItem("Debug / Stats", nullptr, &m_show_debug);
                ImGui::MenuItem("ImGui Demo", nullptr, &m_show_imgui_demo);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (m_show_inspector) {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();

            constexpr float panel_width = 360.0f;
            constexpr float top_offset = 24.0f;

            ImGui::SetNextWindowPos(
                ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - panel_width, viewport->WorkPos.y + top_offset),
                ImGuiCond_FirstUseEver
            );

            ImGui::SetNextWindowSize(
                ImVec2(panel_width, 260.0f),
                ImGuiCond_FirstUseEver
            );

            if (ImGui::Begin("Inspector", &m_show_inspector)) {
                scene.draw_inspector_ui();
            }
            ImGui::End();
        }

        if (m_show_debug) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 34.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(220.0f, 130.0f), ImGuiCond_FirstUseEver);

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