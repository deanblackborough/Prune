#pragma once

#include <imgui.h>

namespace prune::tooling::imgui::layout {

    // Section headers
    bool collapsing_header(const char* label, bool default_open = true);

    // Visual separators
    void separator();
    void spacing(int count = 1);

    // Text blocks
    void header_text(const char* text);
    void text(const char* text);
    void text_wrapped(const char* text);
} 