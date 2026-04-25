#include "prune/tooling/theme.hpp"

#include "imgui.h"

namespace prune::tooling {

    void apply_editor_theme(EditorTheme theme)
    {
        switch (theme) {
        case EditorTheme::PrunePurple:
            break;
        }

        ImGuiStyle& style = ImGui::GetStyle();

        style.WindowRounding = 3.0f;
        style.ChildRounding = 3.0f;
        style.FrameRounding = 3.0f;
        style.PopupRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.TabRounding = 3.0f;

        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.FramePadding = ImVec2(4.0f, 2.0f);
        style.FrameBorderSize = 1.0f;
        style.ItemSpacing = ImVec2(4.0f, 2.0f);
        style.ItemInnerSpacing = ImVec2(3.0f, 2.0f);
        style.IndentSpacing = 18.0f;
        style.ScrollbarSize = 8.0f;
        style.GrabMinSize = 10.0f;

        ImVec4* colors = style.Colors;

        const ImVec4 bg = ImVec4(0.075f, 0.060f, 0.095f, 1.0f);
        const ImVec4 panel = ImVec4(0.110f, 0.085f, 0.145f, 1.0f);
        const ImVec4 panel_light = ImVec4(0.155f, 0.115f, 0.205f, 1.0f);
        const ImVec4 purple = ImVec4(0.450f, 0.240f, 0.660f, 1.0f);
        const ImVec4 purple_hot = ImVec4(0.520f, 0.290f, 0.760f, 1.0f);
        const ImVec4 purple_dim = ImVec4(0.360f, 0.190f, 0.560f, 1.0f);
        const ImVec4 text = ImVec4(0.86f, 0.82f, 0.90f, 1.0f);
        const ImVec4 text_muted = ImVec4(0.55f, 0.50f, 0.62f, 1.0f);

        colors[ImGuiCol_Text] = text;
        colors[ImGuiCol_TextDisabled] = text_muted;

        colors[ImGuiCol_WindowBg] = bg;
        colors[ImGuiCol_ChildBg] = panel;
        colors[ImGuiCol_PopupBg] = panel;

        colors[ImGuiCol_Border] = ImVec4(0.300f, 0.210f, 0.400f, 1.0f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

        colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.13f, 0.24f, 1.0f);
        colors[ImGuiCol_FrameBgHovered] = purple_dim;
        colors[ImGuiCol_FrameBgActive] = purple;

        colors[ImGuiCol_TitleBg] = panel;
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.30f, 0.17f, 0.46f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed] = panel;

        colors[ImGuiCol_MenuBarBg] = panel;

        colors[ImGuiCol_ScrollbarBg] = bg;
        colors[ImGuiCol_ScrollbarGrab] = purple_dim;
        colors[ImGuiCol_ScrollbarGrabHovered] = purple;
        colors[ImGuiCol_ScrollbarGrabActive] = purple_hot;

        colors[ImGuiCol_CheckMark] = purple_hot;
        colors[ImGuiCol_SliderGrab] = purple;
        colors[ImGuiCol_SliderGrabActive] = purple_hot;

        colors[ImGuiCol_Button] = purple_dim;
        colors[ImGuiCol_ButtonHovered] = purple;
        colors[ImGuiCol_ButtonActive] = purple_hot;

        colors[ImGuiCol_Header] = ImVec4(0.25f, 0.14f, 0.38f, 1.0f);
        colors[ImGuiCol_HeaderHovered] = purple;
        colors[ImGuiCol_HeaderActive] = purple_hot;

        colors[ImGuiCol_Separator] = ImVec4(0.300f, 0.210f, 0.400f, 1.0f);
        colors[ImGuiCol_SeparatorHovered] = purple;
        colors[ImGuiCol_SeparatorActive] = purple_hot;

        colors[ImGuiCol_ResizeGrip] = purple_dim;
        colors[ImGuiCol_ResizeGripHovered] = purple;
        colors[ImGuiCol_ResizeGripActive] = purple_hot;

        colors[ImGuiCol_Tab] = panel_light;
        colors[ImGuiCol_TabHovered] = purple;
        colors[ImGuiCol_TabActive] = purple_dim;
        colors[ImGuiCol_TabUnfocused] = panel;
        colors[ImGuiCol_TabUnfocusedActive] = panel_light;

        colors[ImGuiCol_NavHighlight] = purple_hot;
    }
}