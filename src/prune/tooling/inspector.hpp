#pragma once

#include <array>
#include <optional>

#include "prune/editor/editor_command.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/editor/tracked_property_table.hpp"

namespace prune {

    class Inspector {
    public:
        void draw(
            Scene& scene,
            const Camera& camera
        );

    private:
        void draw_selected(Scene& scene);
        void draw_scene_meaning(Scene& scene);
        void draw_properties(Scene& scene, GameObjectManager& objects);
        void draw_computed(GameObjectManager& objects, const Camera& camera);
        void draw_flags(Scene& scene, GameObjectManager& objects);

        void sync_rename_buffer(const GameObject* selected);
        std::optional<GameObjectId> m_rename_target_id;
        tooling::editor::tracked_property_table::ObjectEditTracker m_object_edit_tracker;
		std::array<char, 128> m_rename_buffer{};
    };
}