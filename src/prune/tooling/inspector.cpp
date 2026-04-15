#include "imgui.h"

#include "prune/tooling/inspector.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Inspector::draw(
        GameObjectManager& objects,
        GameObjectId player_id,
        PlayerController& player_controller,
        GridOptions& grid_options
    ) {
        draw_selected(objects, player_id);
        draw_properties(objects, player_id, player_controller, grid_options);
        draw_computed(objects);
        draw_flags(objects, player_id);
    }

    void Inspector::draw_selected(
        GameObjectManager& objects,
        GameObjectId player_id
    ) {
        GameObject* selected = objects.selected_object();

        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (ImGui::CollapsingHeader("Selected", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (tooling::imgui::property_table::begin("Selected")) {

                tooling::imgui::property_table::text("Id", std::to_string(selected->id).c_str());

                if (is_player) {
                    tooling::imgui::property_table::text("Name", selected->name.c_str());
                }
                else {
                    char name_buffer[128]{};
                    std::snprintf(name_buffer, sizeof(name_buffer), "%s", selected->name.c_str());

                    tooling::imgui::property_table::input_text("Name", "##name", name_buffer, sizeof(name_buffer));

                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        selected->name = objects.make_unique_name(name_buffer, selected->id);
                    }
                }

                if (!is_player) {

                    ImGui::Separator();

                    tooling::imgui::property_table::begin_row("Actions");

                    if (tooling::imgui::property_table::button_raw("Delete")) {
                        const GameObjectId id_to_remove = selected->id;
                        objects.remove_object(id_to_remove);

                        tooling::imgui::property_table::end();
                        return;
                    }

                    ImGui::SameLine();

                    if (tooling::imgui::property_table::button_raw("Clone")) {
                        const std::string source_name = selected->name;

                        GameObject clone = *selected;
                        clone.is_player = false;
                        clone.transform.x += 32.0f;
                        clone.transform.y += 32.0f;

                        const GameObjectId clone_id = objects.create_object(clone);

                        if (GameObject* created = objects.get_by_id(clone_id)) {
                            created->name = objects.make_unique_name(source_name, clone_id);
                        }

                        objects.select(clone_id);

                        tooling::imgui::property_table::end();
                        return;
                    }
                }

                tooling::imgui::property_table::end();
            }
        }
    }

    void Inspector::draw_properties(
        GameObjectManager& objects,
        GameObjectId player_id,
        PlayerController& player_controller,
        GridOptions& grid_options
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (tooling::imgui::property_table::begin("##transform")) {
                tooling::imgui::property_table::slider_float("X", "##x", selected->transform.x, -4096.0f, 4096.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Y", "##y", selected->transform.y, -4096.0f, 4096.0f, "%.2f");
                tooling::imgui::property_table::end();
            }
        }

        if (ImGui::CollapsingHeader("Size", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (tooling::imgui::property_table::begin("##size")) {
                tooling::imgui::property_table::slider_int("Width", "##width", selected->rectangle.width, 16, 256);
                tooling::imgui::property_table::slider_int("Height", "##height", selected->rectangle.height, 16, 256);
                tooling::imgui::property_table::end();
            }
        }

        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (tooling::imgui::property_table::begin("##rendering")) {
                tooling::imgui::property_table::color3("Colour", "##colour", selected->rectangle.color);
                tooling::imgui::property_table::end();
            }
        }

        if (is_player) {
            if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
                if (tooling::imgui::property_table::begin("##player")) {
                    float speed = player_controller.speed();
                    tooling::imgui::property_table::slider_float("Speed", "##speed", speed, 0.0f, 512.0f, "%.2f");
                    tooling::imgui::property_table::end();
                }
            }
        }

        if (grid_options.snap_to_grid && !is_player) {
            // @todo We need to get this working
            //snap_object_to_grid(*selected);
        }
 
    }

    void Inspector::draw_computed(
        GameObjectManager& objects
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        /*
         *@todo This is a hack until we have the camera structs
         */
        int camera_x = 0;
        int camera_y = 0;

        if (ImGui::CollapsingHeader("Computed")) {
            const Transform screen_pos = {
                selected->transform.x - camera_x,
                selected->transform.y - camera_y
            };

            ImGui::Text("Screen Position: %.1f, %.1f", screen_pos.x, screen_pos.y);
        }
    }

    void Inspector::draw_flags(
        GameObjectManager& objects,
        GameObjectId player_id
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (ImGui::CollapsingHeader("Flags")) {
            ImGui::Checkbox("Active", &selected->active);
            ImGui::Checkbox("Visible", &selected->visible);

            if (is_player) {
                ImGui::BeginDisabled();
                ImGui::Checkbox("Solid", &selected->solid);
                ImGui::EndDisabled();

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                    ImGui::SetTooltip("Player solid value not used yet, player checks against game objects only");
                }

                bool player_flag = true;
                ImGui::BeginDisabled();
                ImGui::Checkbox("IsPlayer", &player_flag);
                ImGui::EndDisabled();
            } else {
                ImGui::Checkbox("Solid", &selected->solid);
            }
        }
    }
}
