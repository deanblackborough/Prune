#include "imgui.h"

#include "prune/tooling/stats.hpp"

namespace prune {

    void Stats::draw(GameObjectManager& objects, GameObjectId player_id) {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Frame time: %.3f ms", 1000.0f / io.Framerate);
        ImGui::Separator();

        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Object count: %d", static_cast<int>(objects.count()));
        ImGui::Text("Selected id: %u", objects.selected_id());
        ImGui::Text("Player id: %u", player_id);

        // @todo We need to make these work
        //ImGui::Text("Grid: %s", show_grid ? "On" : "Off");
        //ImGui::Text("Snap: %s", snap_to_grid ? "On" : "Off");
        //ImGui::Text("Grid size: %d", grid_size);
    }
}
