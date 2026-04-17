#include "prune/tooling/imgui/layout.hpp"

namespace prune::tooling::imgui::layout {

    bool collapsing_header(const char* label, bool default_open) {
        ImGuiTreeNodeFlags flags = default_open ? ImGuiTreeNodeFlags_DefaultOpen : 0;
        return ImGui::CollapsingHeader(label, flags);
    }

    void separator() {
        ImGui::Separator();
    }

    void text(const char* text) {
        ImGui::TextUnformatted(text);
    }

    void text_wrapped(const char* text) {
        ImGui::TextWrapped("%s", text);
    }
}