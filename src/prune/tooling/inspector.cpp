#include "imgui.h"

#include "inspector.hpp"

namespace prune {

    void Inspector::draw(
        GameObjectManager& objects,
        GameObjectId player_id,
        PlayerController& player_controller
    ) {
        draw_selected(objects, player_id);
        draw_actions(objects, player_id);
        draw_properties(objects, player_id, player_controller);
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

        ImGui::Text("Id: %u", selected->id);

        if (is_player) {
            ImGui::Text("Name: %s", selected->name.c_str());
        } else {
            char name_buffer[128]{};
            std::snprintf(name_buffer, sizeof(name_buffer), "%s", selected->name.c_str());

            ImGui::InputText("Name", name_buffer, sizeof(name_buffer));
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                selected->name = objects.make_unique_name(name_buffer, selected->id);
            }
        }
    }

    void Inspector::draw_actions(
        GameObjectManager& objects,
        GameObjectId player_id
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (!is_player) {
            if (ImGui::Button("Delete")) {
                const GameObjectId id_to_remove = selected->id;
                objects.remove_object(id_to_remove);
                return;
            }

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

            if (ImGui::Button("Clone")) {
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

                ImGui::PopStyleColor(3);
                return;
            }
            ImGui::PopStyleColor(3);
        }
    }

    void Inspector::draw_properties(
        GameObjectManager& objects,
        GameObjectId player_id,
        PlayerController& player_controller
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (is_player) {
                float speed = player_controller.speed();
                if (ImGui::SliderFloat("Speed", &speed, 32.0f, 512.0f)) {
                    player_controller.set_speed(speed);
                }
            }

            ImGui::DragFloat("Object X", &selected->transform.x, 1.0f);
            ImGui::DragFloat("Object Y", &selected->transform.y, 1.0f);

            /*
             * @todo We need to get this to work
            if (snap_to_grid && !is_player) {
                snap_object_to_grid(*selected);
            }
            */

            ImGui::SliderInt("Width", &selected->rectangle.width, 16, 256);
            ImGui::SliderInt("Height", &selected->rectangle.height, 16, 256);
            ImGui::ColorEdit3("Colour", selected->rectangle.color);
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
