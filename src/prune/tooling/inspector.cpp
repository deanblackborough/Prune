#include <algorithm>
#include <string_view>

#include "imgui.h"

#include "prune/tooling/inspector.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/resources/sprites.hpp"

namespace prune {

    void Inspector::draw(
        Scene& scene,
        GridOptions& grid_options,
        const Camera& camera
    ) {
        GameObjectManager& objects = scene.get_object_manager();

        draw_selected(scene, grid_options);
        draw_scene_meaning(scene);
        draw_properties(objects);
        draw_computed(objects, camera);
        draw_flags(objects);
    }

    void Inspector::draw_selected(
        Scene& scene,
        GridOptions& grid_options
    ) {
        GameObjectManager& objects = scene.get_object_manager();
        GameObject* selected = objects.selected_object();

        if (!selected) {
            return;
        }

        const ObjectConcept object_concept = scene.object_concept_for(*selected);
        const bool can_edit = object_concept.editable && !object_concept.runtime_only;
        const bool can_rename = selected->editor.renameable && can_edit;
        const bool can_delete = selected->editor.deletable && can_edit;
        const bool can_clone = selected->editor.cloneable && can_edit;

        if (tooling::imgui::layout::collapsing_header("Selected")) {
            if (tooling::imgui::property_table::begin("Selected")) {

                tooling::imgui::property_table::text("Id", std::to_string(selected->identity.id).c_str());

                if (!can_rename) {
                    tooling::imgui::property_table::text("Name", selected->identity.name.c_str());
                } else {
                    sync_rename_buffer(selected);

                    tooling::imgui::property_table::input_text(
                        "Name",
                        "##name",
                        m_rename_buffer.data(),
                        m_rename_buffer.size()
                    );

                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        selected->identity.name = objects.make_unique_name(
                            m_rename_buffer.data(),
                            selected->identity.id
                        );

                        std::snprintf(
                            m_rename_buffer.data(),
                            m_rename_buffer.size(),
                            "%s",
                            selected->identity.name.c_str()
                        );
                    }
                }

                if (can_delete || can_clone) {

                    tooling::imgui::layout::separator();

                    tooling::imgui::property_table::begin_row("Actions");

                    if (can_delete && tooling::imgui::property_table::button_raw("Delete")) {
                        const GameObjectId id_to_remove = selected->identity.id;
                        objects.remove_object(id_to_remove);

                        tooling::imgui::property_table::end();
                        return;
                    }

                    if (can_delete && can_clone) {
                        ImGui::SameLine();
                    }

                    if (can_clone && tooling::imgui::property_table::button_raw("Clone")) {
                        const std::string source_name = selected->identity.name;

                        GameObject clone = *selected;

                        const float step = grid_options.snap_to_grid
                            ? static_cast<float>(std::max(1, grid_options.grid_size))
                            : static_cast<float>(k_default_object_size);

                        clone.transform.x += step;
                        clone.transform.y += step;

                        const GameObjectId clone_id = objects.create_object(clone);

                        if (GameObject* created = objects.get_by_id(clone_id)) {
                            created->identity.name = objects.make_unique_name(source_name, clone_id);
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

    void Inspector::draw_scene_meaning(Scene& scene)
    {
        GameObject* selected = scene.get_object_manager().selected_object();
        if (!selected) {
            return;
        }

        if (tooling::imgui::layout::collapsing_header("Scene Meaning")) {
            if (tooling::imgui::property_table::begin("##scene_meaning")) {
                const ObjectConcept object_concept = scene.object_concept_for(*selected);

                tooling::imgui::property_table::text("Concept", object_concept.label.data());
                tooling::imgui::property_table::text("Runtime Created", object_concept.runtime_only ? "Yes" : "No");
                tooling::imgui::property_table::text("Selectable", object_concept.selectable ? "Yes" : "No");
                tooling::imgui::property_table::text("Editable", object_concept.editable ? "Yes" : "No");
                tooling::imgui::property_table::text_wrapped("Purpose", object_concept.purpose.data());
                tooling::imgui::property_table::text_wrapped("Collision Rule", object_concept.collision_rule.data());

                tooling::imgui::property_table::end();
            }
        }
    }

    void Inspector::draw_properties(GameObjectManager& objects) {
        GameObject* selected = objects.selected_object();
        if (!selected) {

            const char* title = "No object selected";
            const char* hint1 = "Click an object in the scene";
            const char* hint2 = "or select one from the Outliner";

            const float window_width = ImGui::GetContentRegionAvail().x;

            auto center_text = [&](const char* text) {
                const float text_width = ImGui::CalcTextSize(text).x;
                ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                ImGui::TextUnformatted(text);
            };

            center_text(title);

            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

            center_text(hint1);
            center_text(hint2);

            ImGui::PopStyleColor();

            ImGui::Spacing();
            ImGui::Separator();

            return;
        }

        if (tooling::imgui::layout::collapsing_header("Transform")) {
            if (tooling::imgui::property_table::begin("##transform")) {
                tooling::imgui::property_table::drag_float("X", "transform_x", selected->transform.x, 1.0f);
                tooling::imgui::property_table::drag_float("Y", "transform_y", selected->transform.y, 1.0f);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Size")) {
            if (tooling::imgui::property_table::begin("##size")) {
                tooling::imgui::property_table::slider_int("Width", "##width", selected->size.width, k_min_object_size, k_max_object_size);
                tooling::imgui::property_table::slider_int("Height", "##height", selected->size.height, k_min_object_size, k_max_object_size);
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Render")) {
            if (tooling::imgui::property_table::begin("##rendering")) {
                int render_type = (selected->render.type == RenderType::Rectangle) ? 0 : 1;
                const char* render_items[] = { "Rectangle", "Sprite" };

                if (tooling::imgui::property_table::combo(
                    "Type",
                    "##render_type",
                    render_type,
                    render_items,
                    IM_ARRAYSIZE(render_items)
                )) {
                    selected->render.type = (render_type == 0)
                        ? RenderType::Rectangle
                        : RenderType::Sprite;
                }

                switch (selected->render.type) {
                case RenderType::Rectangle:
                    tooling::imgui::property_table::color3(
                        "Colour",
                        "##colour",
                        selected->render.rectangle.color
                    );
                    break;

                case RenderType::Sprite:
                    const auto& resources = sprite_resources();

                    std::vector<const char*> sprite_items;
                    sprite_items.reserve(resources.size());

                    int current_sprite = 0;

                    for (int i = 0; i < static_cast<int>(resources.size()); ++i) {
                        sprite_items.push_back(resources[i].key.data());

                        if (selected->render.sprite.sprite_key == resources[i].key) {
                            current_sprite = i;
                        }
                    }

                    if (!sprite_items.empty()) {
                        if (tooling::imgui::property_table::combo(
                            "Sprite",
                            "##sprite_key",
                            current_sprite,
                            sprite_items.data(),
                            static_cast<int>(sprite_items.size())
                        )) {
                            selected->render.sprite.sprite_key = resources[current_sprite].key;
                        }
                    }
                    else {
                        tooling::imgui::property_table::text("Sprite", "No sprites defined");
                    }

                    break;
                }

                tooling::imgui::property_table::end();
            }
        }
    }

    void Inspector::draw_computed(
        GameObjectManager& objects,
        const Camera& camera
    ) {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            return;
        }

        if (tooling::imgui::layout::collapsing_header("Computed", false)) {
            if (tooling::imgui::property_table::begin("##computed")) {
                const Transform screen_pos = {
                    selected->transform.x - camera.x,
                    selected->transform.y - camera.y
                };

                char screen_pos_buffer[64];
                std::snprintf(
                    screen_pos_buffer,
                    sizeof(screen_pos_buffer),
                    "x %.1f, y %.1f",
                    screen_pos.x,
                    screen_pos.y
                );

                tooling::imgui::property_table::text("Screen Position", screen_pos_buffer);
                tooling::imgui::property_table::end();
            }
        }
    }

    void Inspector::draw_flags(GameObjectManager& objects)
    {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            return;
        }

        if (tooling::imgui::layout::collapsing_header("Collision / Editor / Lifecycle", false)) {
            if (tooling::imgui::property_table::begin("##flags")) {
                tooling::imgui::property_table::checkbox("Lifecycle Active", "##active", selected->lifecycle.active);
                tooling::imgui::property_table::checkbox("Render Visible", "##visible", selected->render.visible);
                tooling::imgui::property_table::checkbox("Collision Solid", "##solid", selected->collision.solid);
                tooling::imgui::property_table::checkbox_readonly("Editor Selectable", "##editor_selectable", selected->editor.selectable);
                tooling::imgui::property_table::checkbox_readonly("Editor Renameable", "##editor_renameable", selected->editor.renameable);
                tooling::imgui::property_table::checkbox_readonly("Editor Cloneable", "##editor_cloneable", selected->editor.cloneable);
                tooling::imgui::property_table::checkbox_readonly("Runtime Persistent", "##runtime_persistent", selected->runtime.persistent);
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

        if (m_rename_target_id.has_value() && m_rename_target_id.value() == selected->identity.id) {
            return;
        }

        m_rename_target_id = selected->identity.id;
        std::snprintf(
            m_rename_buffer.data(),
            m_rename_buffer.size(),
            "%s",
            selected->identity.name.c_str()
        );
    }
}
