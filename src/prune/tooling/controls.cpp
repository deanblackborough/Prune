#include "imgui.h"

#include "prune/tooling/controls.hpp"

namespace prune {

    void Controls::draw() {
        ImGui::TextWrapped("WASD keys move the player");
        ImGui::TextWrapped("Arrow keys move selected non-player object");
        ImGui::TextWrapped("Hold Shift for larger movements");
        ImGui::TextWrapped("IJKL keys move the editor camera");
    }
}
