#include <format>
#include <string>

#include "prune/tooling/stats.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

	void Stats::draw(
		GameObjectManager& objects,
		GameObjectId player_id,
		int viewport_width,
		int viewport_height,
		const CameraState& cameras
	)
	{
        ImGuiIO& io = ImGui::GetIO();

        if (tooling::imgui::layout::collapsing_header("Performance")) {
			tooling::imgui::property_table::begin("##performance");

			const float fps = io.Framerate;
			const float frame_time_ms = (fps > 0.0f) ? (1000.0f / fps) : 0.0f;

			tooling::imgui::property_table::text("FPS", std::to_string(fps).c_str());
			tooling::imgui::property_table::text("Frame time", std::to_string(frame_time_ms).c_str());
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

		if (tooling::imgui::layout::collapsing_header("Camera")) {
			tooling::imgui::property_table::begin("##camera");
			tooling::imgui::property_table::text("Active", cameras.mode == CameraMode::Editor ? "Editor" : "Game");
			tooling::imgui::property_table::text("Editor X", std::format("{:.2f}", cameras.editor.x).c_str());
			tooling::imgui::property_table::text("Editor Y", std::format("{:.2f}", cameras.editor.y).c_str());
			tooling::imgui::property_table::text("Game X", std::format("{:.2f}", cameras.game.x).c_str());
			tooling::imgui::property_table::text("Game Y", std::format("{:.2f}", cameras.game.y).c_str());
			tooling::imgui::property_table::text("Follow player", cameras.game_options.follow_player ? "true" : "false");

			if (const GameObject* player = objects.get_by_id(player_id)) {
				tooling::imgui::property_table::text("Player X", std::format("{:.2f}", player->transform.x).c_str());
				tooling::imgui::property_table::text("Player Y", std::format("{:.2f}", player->transform.y).c_str());
			}

			tooling::imgui::property_table::end();
		}
    }
}
