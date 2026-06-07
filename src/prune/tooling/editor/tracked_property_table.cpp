#include "imgui.h"

#include "prune/tooling/editor/tracked_property_table.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/sprite_picker.hpp"

namespace prune::tooling::editor::tracked_property_table {

    namespace {

        [[nodiscard]] bool command_object_changed(
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
                    before.render.visible != after.render.visible ||
                    before.collision.solid != after.collision.solid;

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

    void ObjectEditTracker::capture_if_activated(const GameObject& before)
    {
        if (ImGui::IsItemActivated()) {
            m_before = before;
        }
    }

    void ObjectEditTracker::commit_if_deactivated_after_edit(
        Scene& scene,
        EditorCommandType type,
        const GameObject& after,
        std::string_view detail
    ) {
        if (!ImGui::IsItemDeactivatedAfterEdit()) {
            return;
        }

        if (!m_before.has_value()) {
            return;
        }

        const GameObject before = m_before.value();
        m_before.reset();

        commit_if_changed(scene, type, before, after, detail);
    }

    void ObjectEditTracker::reset() noexcept
    {
        m_before.reset();
    }

    bool drag_float(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        float& value,
        float speed,
        float min,
        float max,
        const char* format,
        std::string_view detail
    ) {
        const GameObject before = object;
        const bool changed = ::prune::tooling::imgui::property_table::drag_float(label, id, value, speed, min, max, format);

        tracker.capture_if_activated(before);
        tracker.commit_if_deactivated_after_edit(scene, type, object, detail);

        return changed;
    }

    bool slider_int(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        int& value,
        int min,
        int max,
        std::string_view detail
    ) {
        const GameObject before = object;
        const bool changed = ::prune::tooling::imgui::property_table::slider_int(label, id, value, min, max);

        tracker.capture_if_activated(before);
        tracker.commit_if_deactivated_after_edit(scene, type, object, detail);

        return changed;
    }

    bool color3(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        float color[3],
        std::string_view detail
    ) {
        const GameObject before = object;
        const bool changed = ::prune::tooling::imgui::property_table::color3(label, id, color);

        tracker.capture_if_activated(before);
        tracker.commit_if_deactivated_after_edit(scene, type, object, detail);

        return changed;
    }

    bool checkbox(
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        bool& value,
        std::string_view detail
    ) {
        const GameObject before = object;

        if (!::prune::tooling::imgui::property_table::checkbox(label, id, value)) {
            return false;
        }

        commit_if_changed(scene, type, before, object, detail);
        return true;
    }

    bool sprite_picker(
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        std::string& sprite_key,
        std::string_view detail
    ) {
        const GameObject before = object;

        if (!::prune::tooling::draw_sprite_picker(label, id, sprite_key)) {
            return false;
        }

        commit_if_changed(scene, type, before, object, detail);
        return true;
    }

    bool commit_if_changed(
        Scene& scene,
        EditorCommandType type,
        const GameObject& before,
        const GameObject& after,
        std::string_view detail
    ) {
        if (before.identity.id != after.identity.id || !command_object_changed(type, before, after)) {
            return false;
        }

        scene.record_editor_command(make_object_command(
            type,
            editor_command_type_label(type),
            before,
            after,
            detail
        ));

        return true;
    }
}
