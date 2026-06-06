#include <algorithm>
#include <string_view>

#include "imgui.h"

#include "prune/tooling/inspector.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/sprite_picker.hpp"

namespace prune {

    void Inspector::draw(
        Scene& scene,
        GridOptions& grid_options,
        const Camera& camera
    ) {
        GameObjectManager& objects = scene.get_object_manager();

        draw_selected(scene, grid_options);
        draw_scene_meaning(scene);
        draw_properties(scene, objects);
        draw_computed(objects, camera);
        draw_flags(scene, objects);
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

        const bool can_edit = scene.object_is_editable(*selected);
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
                        const GameObject before = *selected;
                        selected->identity.name = objects.make_unique_name(
                            m_rename_buffer.data(),
                            selected->identity.id
                        );

                        if (before.identity.name != selected->identity.name) {
                            scene.record_editor_command(make_object_command(
                                EditorCommandType::RenameObject,
                                editor_command_type_label(EditorCommandType::RenameObject),
                                before,
                                *selected
                            ));
                        }

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
                        const GameObject deleted = *selected;
                        const GameObjectId id_to_remove = selected->identity.id;
                        if (objects.remove_object(id_to_remove)) {
                            scene.record_editor_command(make_delete_object_command(deleted));
                        }

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
                            scene.record_editor_command(make_create_object_command(*created));
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

    void Inspector::draw_properties(Scene& scene, GameObjectManager& objects) {
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

        const bool can_edit = scene.object_is_editable(*selected);
        const bool can_move = scene.object_is_movable(*selected);

        if (!can_edit) {
            ImGui::TextWrapped("This object is protected by the scene and cannot be edited from the generic inspector.");
        }

        if (tooling::imgui::layout::collapsing_header("Transform")) {
            if (tooling::imgui::property_table::begin("##transform")) {
                ImGui::BeginDisabled(!can_move);

                GameObject before = *selected;
                tooling::imgui::property_table::drag_float("X", "transform_x", selected->transform.x, 1.0f);
                capture_object_edit_start(before);
                commit_object_edit(scene, EditorCommandType::ChangeObjectPosition, *selected);

                before = *selected;
                tooling::imgui::property_table::drag_float("Y", "transform_y", selected->transform.y, 1.0f);
                capture_object_edit_start(before);
                commit_object_edit(scene, EditorCommandType::ChangeObjectPosition, *selected);

                ImGui::EndDisabled();
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Size")) {
            if (tooling::imgui::property_table::begin("##size")) {
                ImGui::BeginDisabled(!can_edit);

                GameObject before = *selected;
                tooling::imgui::property_table::slider_int("Width", "##width", selected->size.width, k_min_object_size, k_max_object_size);
                capture_object_edit_start(before);
                commit_object_edit(scene, EditorCommandType::ChangeObjectSize, *selected);

                before = *selected;
                tooling::imgui::property_table::slider_int("Height", "##height", selected->size.height, k_min_object_size, k_max_object_size);
                capture_object_edit_start(before);
                commit_object_edit(scene, EditorCommandType::ChangeObjectSize, *selected);

                ImGui::EndDisabled();
                tooling::imgui::property_table::end();
            }
        }

        if (tooling::imgui::layout::collapsing_header("Render")) {
            if (tooling::imgui::property_table::begin("##rendering")) {
                ImGui::BeginDisabled(!can_edit);
                int render_type = (selected->render.type == RenderType::Rectangle) ? 0 : 1;
                const char* render_items[] = { "Rectangle", "Sprite" };

                if (tooling::imgui::property_table::combo(
                    "Type",
                    "##render_type",
                    render_type,
                    render_items,
                    IM_ARRAYSIZE(render_items)
                )) {
                    const GameObject before = *selected;
                    selected->render.type = (render_type == 0)
                        ? RenderType::Rectangle
                        : RenderType::Sprite;

                    scene.record_editor_command(make_object_command(
                        EditorCommandType::ChangeObjectRenderType,
                        editor_command_type_label(EditorCommandType::ChangeObjectRenderType),
                        before,
                        *selected
                    ));
                }

                switch (selected->render.type) {
                case RenderType::Rectangle:
                    {
                        const GameObject before = *selected;
                        tooling::imgui::property_table::color3(
                            "Colour",
                            "##colour",
                            selected->render.rectangle.color
                        );
                        capture_object_edit_start(before);
                        commit_object_edit(scene, EditorCommandType::ChangeObjectColour, *selected);
                    }
                    break;

                case RenderType::Sprite:
                    {
                        const GameObject before = *selected;
                        if (tooling::draw_sprite_picker(
                            "Sprite",
                            "##sprite_key",
                            selected->render.sprite.sprite_key
                        )) {
                            scene.record_editor_command(make_object_command(
                                EditorCommandType::ChangeSprite,
                                editor_command_type_label(EditorCommandType::ChangeSprite),
                                before,
                                *selected
                            ));
                        }
                    }
                    {
                        const GameObject before = *selected;
                        if (tooling::imgui::property_table::checkbox(
                            "Flip X",
                            "##sprite_flip_x",
                            selected->render.sprite.flip_x
                        )) {
                            scene.record_editor_command(make_object_command(
                                EditorCommandType::ChangeSprite,
                                editor_command_type_label(EditorCommandType::ChangeSprite),
                                before,
                                *selected
                            ));
                        }
                    }
                    break;
                }

                ImGui::EndDisabled();
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

    void Inspector::draw_flags(Scene& scene, GameObjectManager& objects)
    {
        GameObject* selected = objects.selected_object();
        if (!selected) {
            return;
        }

        if (tooling::imgui::layout::collapsing_header("Collision / Editor / Lifecycle", false)) {
            if (tooling::imgui::property_table::begin("##flags")) {
                const bool can_edit = scene.object_is_editable(*selected);
                ImGui::BeginDisabled(!can_edit);

                {
                    const GameObject before = *selected;
                    if (tooling::imgui::property_table::checkbox(
                        "Lifecycle Active",
                        "##active",
                        selected->lifecycle.active
                    ))
                    {
                        scene.record_editor_command(make_object_command(
                            EditorCommandType::ChangeObjectFlag,
                            editor_command_type_label(EditorCommandType::ChangeObjectFlag),
                            before,
                            *selected
                        ));
                    }
                }

                {
                    const GameObject before = *selected;
                    if (tooling::imgui::property_table::checkbox(
                        "Render Visible",
                        "##visible",
                        selected->render.visible
                    )) {
                        scene.record_editor_command(make_object_command(
                            EditorCommandType::ChangeObjectFlag,
                            editor_command_type_label(EditorCommandType::ChangeObjectFlag),
                            before,
                            *selected
                        ));
                    }
                }

                tooling::imgui::property_table::checkbox("Collision Solid", "##solid", selected->collision.solid);
                ImGui::EndDisabled();
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

    void Inspector::capture_object_edit_start(const GameObject& before)
    {
        if (ImGui::IsItemActivated()) {
            m_pending_object_edit = before;
        }
    }

    void Inspector::commit_object_edit(Scene& scene, EditorCommandType type, const GameObject& after)
    {
        if (!ImGui::IsItemDeactivatedAfterEdit()) {
            return;
        }

        if (!m_pending_object_edit.has_value()) {
            return;
        }

        const GameObject before = m_pending_object_edit.value();
        m_pending_object_edit.reset();

        if (before.identity.id != after.identity.id || !command_object_changed(type, before, after)) {
            return;
        }

        scene.record_editor_command(make_object_command(
            type,
            editor_command_type_label(type),
            before,
            after
        ));
    }

    bool Inspector::command_object_changed(
        EditorCommandType type,
        const GameObject& before,
        const GameObject& after
    ) noexcept {
        switch (type) {
        case EditorCommandType::MoveObject:
        case EditorCommandType::ChangeObjectPosition:
            return before.transform.x != after.transform.x || before.transform.y != after.transform.y;

        case EditorCommandType::ChangeObjectSize:
            return before.size.width != after.size.width || before.size.height != after.size.height;

        case EditorCommandType::ChangeObjectRenderType:
            return before.render.type != after.render.type;

        case EditorCommandType::ChangeObjectColour:
            return before.render.rectangle.color[0] != after.render.rectangle.color[0] ||
                before.render.rectangle.color[1] != after.render.rectangle.color[1] ||
                before.render.rectangle.color[2] != after.render.rectangle.color[2];

        case EditorCommandType::ChangeObjectFlag:
            return before.lifecycle.active != after.lifecycle.active ||
                before.render.visible != after.render.visible;

        case EditorCommandType::ChangeSprite:
            return before.render.sprite.sprite_key != after.render.sprite.sprite_key ||
                before.render.sprite.flip_x != after.render.sprite.flip_x;

        case EditorCommandType::RenameObject:
            return before.identity.name != after.identity.name;

        case EditorCommandType::CreateObject:
        case EditorCommandType::DeleteObject:
        case EditorCommandType::MoveViewport:
            return true;
        }

        return true;
    }

}
