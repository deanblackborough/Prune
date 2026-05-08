#include <algorithm>

#include "imgui.h"

#include "prune/tooling/editor_layout.hpp"

namespace prune::tooling {

    namespace {

        constexpr float k_margin = 5.0f;
        constexpr float k_menu_bar_height = 25.0f;

        constexpr float k_right_panel_width = 330.0f;

        constexpr float k_outliner_height = 175.0f;
        constexpr float k_simple_shooter_height = 180.0f;
        constexpr float k_inspector_height = 330.0f;

        constexpr float k_stats_width = 300.0f;
        constexpr float k_stats_height = 320.0f;

        constexpr float k_options_width = 306.0f;
        constexpr float k_options_height = 308.0f;

        constexpr float k_controls_width = 377.0f;
        constexpr float k_controls_height = 165.0f;

        constexpr float k_scene_min_width = 320.0f;
        constexpr float k_scene_min_height = 240.0f;

        [[nodiscard]] ImGuiViewport* main_viewport()
        {
            return ImGui::GetMainViewport();
        }

        [[nodiscard]] ImVec2 work_pos()
        {
            return main_viewport()->WorkPos;
        }

        [[nodiscard]] ImVec2 work_size()
        {
            return main_viewport()->WorkSize;
        }

        [[nodiscard]] float right_panel_x()
        {
            const ImVec2 pos = work_pos();
            const ImVec2 size = work_size();

            return pos.x + std::max(k_margin, size.x - k_right_panel_width - k_margin);
        }

        [[nodiscard]] float top_y()
        {
            return work_pos().y + k_menu_bar_height;
        }

        [[nodiscard]] ImVec2 centered_position()
        {
            return main_viewport()->GetCenter();
        }

        void set_window_default(const ImVec2 position, const ImVec2 size)
        {
            ImGui::SetNextWindowPos(position, ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(size, ImGuiCond_FirstUseEver);
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
        const ImVec2 pos = work_pos();
        const ImVec2 size = work_size();

        const float right_panel_start_x = right_panel_x();

        const float scene_x = pos.x + k_margin;
        const float scene_y = top_y();

        const float scene_width = std::max(
            k_scene_min_width,
            right_panel_start_x - scene_x - k_margin
        );

        const float scene_height = std::max(
            k_scene_min_height,
            size.y - k_menu_bar_height - k_margin
        );

        set_window_default(
            ImVec2(scene_x, scene_y),
            ImVec2(scene_width, scene_height)
        );
    }

    void EditorLayout::outliner()
    {
        set_window_default(
            ImVec2(right_panel_x(), top_y()),
            ImVec2(k_right_panel_width, k_outliner_height)
        );
    }

    void EditorLayout::simple_shooter()
    {
        set_window_default(
            ImVec2(right_panel_x(), top_y() + k_outliner_height + k_margin),
            ImVec2(k_right_panel_width, k_simple_shooter_height)
        );
    }

    void EditorLayout::inspector()
    {
        set_window_default(
            ImVec2(
                right_panel_x(),
                top_y() + k_outliner_height + k_simple_shooter_height + (k_margin * 2.0f)
            ),
            ImVec2(k_right_panel_width, k_inspector_height)
        );
    }

    void EditorLayout::stats()
    {
        set_window_default(
            ImVec2(work_pos().x + 10.0f, work_pos().y + 34.0f),
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