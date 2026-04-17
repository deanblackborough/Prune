#include "prune/tooling/controls.hpp"
#include "prune/tooling/imgui/layout.hpp"

namespace prune {

    void Controls::draw() {
		tooling::imgui::layout::text_wrapped("Player");
		tooling::imgui::layout::text_wrapped("WASD keys move the player");

		tooling::imgui::layout::separator();

		tooling::imgui::layout::text_wrapped("Objects");
		tooling::imgui::layout::text_wrapped("Arrow keys move selected non-player object");
		tooling::imgui::layout::text_wrapped("Hold Shift for larger movements");

		tooling::imgui::layout::separator();

		tooling::imgui::layout::text_wrapped("Camera");
		tooling::imgui::layout::text_wrapped("IJKL keys move the editor camera");
    }
}
