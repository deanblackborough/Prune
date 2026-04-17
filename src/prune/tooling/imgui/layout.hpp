#pragma once

#include <imgui.h>

namespace prune::tooling::imgui::layout {

    // Section headers
    bool collapsing_header(const char* label, bool default_open = true);

    // Visual separators
    void separator();

    // Text blocks
    void text(const char* text);
    void text_wrapped(const char* text);
} 