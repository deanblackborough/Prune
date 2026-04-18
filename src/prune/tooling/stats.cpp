#include "prune/tooling/stats.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Stats::draw(GameObjectManager& objects, GameObjectId player_id, int viewport_width, int viewport_height) {
        ImGuiIO& io = ImGui::GetIO();

        if (tooling::imgui::layout::collapsing_header("Performance")) {
			tooling::imgui::property_table::begin("##performance");
			tooling::imgui::property_table::text("FPS", std::to_string(io.Framerate).c_str());
			tooling::imgui::property_table::text("Frame time", std::to_string(1000.0f / io.Framerate).c_str());
			tooling::imgui::property_table::end();
        }

        if (tooling::imgui::layout::collapsing_header("Scene")) {
			tooling::imgui::property_table::begin("##scene");
			tooling::imgui::property_table::text("Object count", std::to_string(objects.count()).c_str());
			tooling::imgui::property_table::text("Selected id", std::to_string(objects.selected_id()).c_str());
			tooling::imgui::property_table::text("Player id", std::to_string(player_id).c_str());
			tooling::imgui::property_table::end();
        }

		if (tooling::imgui::layout::collapsing_header("Viewport")) {
			tooling::imgui::property_table::begin("##viewport");
			tooling::imgui::property_table::text("Width", std::to_string(viewport_width).c_str());
			tooling::imgui::property_table::text("Height", std::to_string(viewport_height).c_str());
			tooling::imgui::property_table::end();
		}
    }
}
