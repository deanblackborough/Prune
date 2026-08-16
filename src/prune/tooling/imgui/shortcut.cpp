#include "prune/tooling/imgui/shortcut.hpp"

namespace prune::tooling::imgui::shortcut {

    bool pressed(ImGuiKeyChord chord)
    {
        const ImGuiIO& io = ImGui::GetIO();

        return !io.WantTextInput &&
            !io.WantCaptureKeyboard &&
            ImGui::Shortcut(chord, ImGuiInputFlags_RouteGlobal);
    }

}
