#pragma once

namespace prune {

    enum class EditorTool {
        Select = 0,
        Move
    };

    [[nodiscard]] inline constexpr const char* editor_tool_label(EditorTool tool) noexcept
    {
        switch (tool) {
        case EditorTool::Select:
            return "Select";
        case EditorTool::Move:
            return "Move";
        }

        return "Unknown";
    }

    [[nodiscard]] inline constexpr bool editor_tool_allows_body_move(EditorTool tool) noexcept
    {
        return tool == EditorTool::Move;
    }

}
