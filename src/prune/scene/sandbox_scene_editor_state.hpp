#pragma once

#include <array>

namespace prune {

    struct SandboxEditorState {
        bool highlight_selected = true;

        std::array<char, 128> object_search{};

        // Grid controls
        bool show_grid = true;
        bool snap_to_grid = false;
        int grid_size = 32;
        int min_grid_size = 16;
        int max_grid_size = 128;
        int nudge_step = 4;
        int shift_nudge_steps = 4;
        int min_nudge_step = 1;
        int max_nudge_step = 32;
    };
}