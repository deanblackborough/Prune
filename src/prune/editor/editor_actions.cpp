#include "prune/editor/editor_actions.hpp"

#include <algorithm>
#include <string>
#include <vector>

#include "prune/editor/editor_command.hpp"

namespace prune {

    namespace {

        [[nodiscard]] float clone_offset(const GridOptions& grid_options) noexcept
        {
            return static_cast<float>(
                grid_options.snap_to_grid
                    ? std::max(1, grid_options.grid_size)
                    : std::max(1, grid_options.nudge_step)
            );
        }

    }

    SelectedObjectActionAvailability selected_object_action_availability(Scene& scene)
    {
        GameObjectManager& objects = scene.get_object_manager();

        SelectedObjectActionAvailability availability{};
        if (GameObject* active = objects.selected_object()) {
            availability.can_clone_active =
                scene.object_is_editable(*active) &&
                active->editor.cloneable;
        }

        for (const GameObjectId id : objects.selected_ids()) {
            const GameObject* object = objects.get_by_id(id);
            if (!object || !scene.object_is_editable(*object) || !object->editor.deletable) {
                continue;
            }

            ++availability.deletable_count;
        }

        availability.can_delete_selection = availability.deletable_count > 0;
        return availability;
    }

    bool clone_active_selected_object(Scene& scene, const GridOptions& grid_options)
    {
        GameObjectManager& objects = scene.get_object_manager();
        const GameObject* selected = objects.selected_object();

        if (!selected || !scene.object_is_editable(*selected) || !selected->editor.cloneable) {
            return false;
        }

        GameObject clone = *selected;
        clone.identity.name = objects.make_unique_name(clone.identity.name + " Copy", k_invalid_game_object_id);

        const float offset = clone_offset(grid_options);
        clone.transform.x += offset;
        clone.transform.y += offset;

        const GameObjectId clone_id = objects.create_object(clone);
        GameObject* created = objects.get_by_id(clone_id);
        if (!created) {
            return false;
        }

        objects.select(clone_id);
        scene.record_editor_command(make_create_object_command(*created, created->identity.name));
        return true;
    }

    bool delete_selected_objects(Scene& scene)
    {
        GameObjectManager& objects = scene.get_object_manager();
        std::vector<GameObject> deleted_objects;
        deleted_objects.reserve(objects.selected_count());

        for (const GameObjectId id : objects.selected_ids()) {
            const GameObject* object = objects.get_by_id(id);
            if (!object || !scene.object_is_editable(*object) || !object->editor.deletable) {
                continue;
            }

            deleted_objects.push_back(*object);
        }

        if (deleted_objects.empty()) {
            return false;
        }

        for (const GameObject& object : deleted_objects) {
            objects.remove_object(object.identity.id);
        }

        if (deleted_objects.size() > 1) {
            objects.clear_selection();
        }

        if (deleted_objects.size() == 1) {
            scene.record_editor_command(make_delete_object_command(deleted_objects.front(), deleted_objects.front().identity.name));
        }
        else {
            scene.record_editor_command(make_multi_delete_object_command(
                deleted_objects,
                std::to_string(deleted_objects.size()) + " objects"
            ));
        }

        return true;
    }

}
