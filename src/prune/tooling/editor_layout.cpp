#include <algorithm>

#include "imgui.h"

#include "prune/tooling/editor_layout.hpp"

namespace prune::tooling {

    namespace {

        constexpr float k_margin = 5.0f;
        constexpr float k_menu_bar_height = 25.0f;

        constexpr float k_right_panel_width = 448.0f;

        constexpr float k_outliner_height = 170.0f;
        constexpr float k_scene_panel_height = 190.0f;
        constexpr float k_min_inspector_height = 260.0f;

        constexpr float k_stats_width = 300.0f;
        constexpr float k_stats_height = 320.0f;

        constexpr float k_options_width = 306.0f;
        constexpr float k_options_height = 308.0f;

        constexpr float k_controls_width = 377.0f;
        constexpr float k_controls_height = 165.0f;

        constexpr float k_scene_min_width = 320.0f;
        constexpr float k_scene_min_height = 240.0f;

        struct LayoutMetrics {
            ImVec2 work_pos;
            ImVec2 work_size;

            float top_y;
            float right_x;
            float right_width;

            float outliner_y;
            float scene_panel_y;
            float inspector_y;
            float inspector_height;
        };

        [[nodiscard]] LayoutMetrics metrics()
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();

            const ImVec2 work_pos = viewport->WorkPos;
            const ImVec2 work_size = viewport->WorkSize;

            const float top_y = work_pos.y + k_menu_bar_height;
            const float right_width = k_right_panel_width;

            const float right_x = work_pos.x + std::max(
                k_margin,
                work_size.x - right_width - k_margin
            );

            const float outliner_y = top_y;
            const float scene_panel_y = outliner_y + k_outliner_height + k_margin;
            const float inspector_y = scene_panel_y + k_scene_panel_height + k_margin;

            const float inspector_height = std::max(
                k_min_inspector_height,
                (work_pos.y + work_size.y) - inspector_y - k_margin
            );

            return LayoutMetrics{
                work_pos,
                work_size,
                top_y,
                right_x,
                right_width,
                outliner_y,
                scene_panel_y,
                inspector_y,
                inspector_height
            };
        }

        [[nodiscard]] ImVec2 centered_position()
        {
            return ImGui::GetMainViewport()->GetCenter();
        }

        void set_window_default(const ImVec2 position, const ImVec2 size)
        {
            //ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
            //ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(position, ImGuiCond_Always);
            ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        }

        void set_centered_window_default(const ImVec2 size)
        {
            ImGui::SetNextWindowPos(
                centered_position(),
                ImGuiCond_FirstUseEver,
                ImVec2(0.5f, 0.5f)
            );

            ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
        }

    }

    void EditorLayout::scene_viewport()
    {
        const LayoutMetrics layout = metrics();

        const float scene_x = layout.work_pos.x + k_margin;
        const float scene_y = layout.top_y;

        const float scene_width = std::max(
            k_scene_min_width,
            layout.right_x - scene_x - k_margin
        );

        const float scene_height = std::max(
            k_scene_min_height,
            layout.work_size.y - k_menu_bar_height - k_margin
        );

        set_window_default(
            ImVec2(scene_x, scene_y),
            ImVec2(scene_width, scene_height)
        );
    }

    void EditorLayout::outliner()
    {
        const LayoutMetrics layout = metrics();

        set_window_default(
            ImVec2(layout.right_x, layout.outliner_y),
            ImVec2(layout.right_width, k_outliner_height)
        );
    }

    void EditorLayout::scene_panel()
    {
        const LayoutMetrics layout = metrics();

        set_window_default(
            ImVec2(layout.right_x, layout.scene_panel_y),
            ImVec2(layout.right_width, k_scene_panel_height)
        );
    }

    void EditorLayout::inspector()
    {
        const LayoutMetrics layout = metrics();

        set_window_default(
            ImVec2(layout.right_x, layout.inspector_y),
            ImVec2(layout.right_width, layout.inspector_height)
        );
    }

    void EditorLayout::stats()
    {
        const LayoutMetrics layout = metrics();

        set_window_default(
            ImVec2(layout.work_pos.x + 10.0f, layout.work_pos.y + 34.0f),
            ImVec2(k_stats_width, k_stats_height)
        );
    }

    void EditorLayout::options()
    {
        set_centered_window_default(ImVec2(k_options_width, k_options_height));
    }

    void EditorLayout::controls()
    {
        set_centered_window_default(ImVec2(k_controls_width, k_controls_height));
    }

}