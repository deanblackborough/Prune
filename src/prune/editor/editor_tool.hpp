#pragma once

namespace prune {

    enum class EditorTool {
        Select = 0,
        Move,
        Scale
    };

    [[nodiscard]] inline constexpr const char* editor_tool_label(EditorTool tool) noexcept
    {
        switch (tool) {
        case EditorTool::Select:
            return "Select";
        case EditorTool::Move:
            return "Move";
        case EditorTool::Scale:
            return "Scale";
        }

        return "Unknown";
    }

    [[nodiscard]] inline constexpr bool editor_tool_allows_body_move(EditorTool tool) noexcept
    {
        return tool == EditorTool::Move;
    }

    [[nodiscard]] inline constexpr bool editor_tool_allows_scale(EditorTool tool) noexcept
    {
        return tool == EditorTool::Scale;
    }

}
