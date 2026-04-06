#pragma once

#include <array>

namespace prune {

    struct SandboxEditorState {
        bool highlight_selected = true;

        std::array<char, 128> object_search{};

        // Grid controls
        bool show_grid = false;
        bool snap_to_grid = false;
        int grid_size = 32;
        int nudge_step = 32;
    };
}