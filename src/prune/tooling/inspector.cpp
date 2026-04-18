#include <algorithm>

#include "imgui.h"

#include "prune/tooling/inspector.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    void Inspector::draw(
        GameObjectManager& objects,
        GameObjectId player_id,
        PlayerController& player_controller,
        GridOptions& grid_options
    ) {
        draw_selected(objects, player_id, grid_options);
        draw_properties(objects, player_id, player_controller, grid_options);
        draw_computed(objects);
        draw_flags(objects, player_id);
    }

    void Inspector::draw_selected(
        GameObjectManager& objects,
        GameObjectId player_id,
        GridOptions& grid_options
    ) {
        GameObject* selected = objects.selected_object();

        if (!selected) {
            tooling::imgui::layout::text("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (tooling::imgui::layout::collapsing_header("Selected")) {
            if (tooling::imgui::property_table::begin("Selected")) {

                tooling::imgui::property_table::text("Id", std::to_string(selected->id).c_str());

                if (is_player) {
                    tooling::imgui::property_table::text("Name", selected->name.c_str());
                }
                else {
                    sync_rename_buffer(selected);

                    tooling::imgui::property_table::input_text(
                        "Name",
                        "##name",
                        m_rename_buffer.data(),
                        m_rename_buffer.size()
                    );

                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        selected->name = objects.make_unique_name(m_rename_buffer.data(), selected->id);
                        std::snprintf(
                            m_rename_buffer.data(),
                            m_rename_buffer.size(),
                            "%s",
                            selected->name.c_str()
                        );
                    }
                }

                if (!is_player) {

                    tooling::imgui::layout::separator();

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

                        const float step = grid_options.snap_to_grid
                            ? static_cast<float>(std::max(1, grid_options.grid_size))
                            : 32.0f;

                        clone.transform.x += step;
                        clone.transform.y += step;

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
            tooling::imgui::layout::text("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (tooling::imgui::layout::collapsing_header("Transform")) {
            if (tooling::imgui::property_table::begin("##transform")) {
                tooling::imgui::property_table::slider_float("X", "##x", selected->transform.x, -4096.0f, 4096.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Y", "##y", selected->transform.y, -4096.0f, 4096.0f, "%.2f");
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Size")) {
            if (tooling::imgui::property_table::begin("##size")) {
                tooling::imgui::property_table::slider_int("Width", "##width", selected->rectangle.width, 16, 256);
                tooling::imgui::property_table::slider_int("Height", "##height", selected->rectangle.height, 16, 256);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Rendering")) {
            if (tooling::imgui::property_table::begin("##rendering")) {
                tooling::imgui::property_table::color3("Colour", "##colour", selected->rectangle.color);
                tooling::imgui::property_table::end();
            }
        }

        if (is_player) {
            if (tooling::imgui::layout::collapsing_header("Player")) {
                if (tooling::imgui::property_table::begin("##player")) {
                    float speed = player_controller.speed();

                    if (tooling::imgui::property_table::slider_float("Speed", "##speed", speed, 0.0f, 512.0f, "%.2f")) {
						player_controller.set_speed(speed);
                    }
                    tooling::imgui::property_table::end();
                }
            }
        }
    }

    void Inspector::draw_computed(
        GameObjectManager& objects
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            tooling::imgui::layout::text("No object selected.");
            return;
        }

        /*
         *@todo This is a hack until we have the camera structs
         */
        int camera_x = 0;
        int camera_y = 0;

        if (tooling::imgui::layout::collapsing_header("Computed", false)) {
            if (tooling::imgui::property_table::begin("##computed")) {
                const Transform screen_pos = {
                    selected->transform.x - camera_x,
                    selected->transform.y - camera_y
                };

                char screen_pos_buffer[64];
                std::snprintf(screen_pos_buffer, sizeof(screen_pos_buffer), "x %.1f, y %.1f", screen_pos.x, screen_pos.y);
                tooling::imgui::property_table::text("Screen Position", screen_pos_buffer);
                tooling::imgui::property_table::end();
            }
        }
    }

    void Inspector::draw_flags(
        GameObjectManager& objects,
        GameObjectId player_id
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            tooling::imgui::layout::text("No object selected.");
            return;
        }

        const bool is_player = (selected->id == player_id);

        if (tooling::imgui::layout::collapsing_header("Flags", false)) {
            if (tooling::imgui::property_table::begin("##computed")) {

                if (is_player) {
                    tooling::imgui::property_table::checkbox_readonly("Is Player", "##is_player", selected->is_player);
                }
				tooling::imgui::property_table::checkbox("Active", "##active", selected->active);
				tooling::imgui::property_table::checkbox("Visible", "##visible", selected->visible);
                if (!is_player) {
                    tooling::imgui::property_table::checkbox("Solid", "##solid", selected->solid);
                }
				tooling::imgui::property_table::end();
            }
        }
    }
    void Inspector::sync_rename_buffer(const GameObject* selected)
    {
        if (!selected) {
            m_rename_target_id.reset();
            m_rename_buffer[0] = '\0';
            return;
        }

        if (m_rename_target_id.has_value() && m_rename_target_id.value() == selected->id) {
            return;
        }

        m_rename_target_id = selected->id;
        std::snprintf(m_rename_buffer.data(), m_rename_buffer.size(), "%s", selected->name.c_str());
    }
}
