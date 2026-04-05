#pragma once

#include <array>

namespace prune {

    struct SandboxEditorState {
        bool highlight_selected = true;
        std::array<char, 128> object_search{};
    };

}