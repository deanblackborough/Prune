#pragma once

#include <array>
#include <optional>

#include "prune/editor/editor_command.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene.hpp"

namespace prune {

    class Inspector {
    public:
        void draw(
            Scene& scene,
            GridOptions& grid_options,
            const Camera& camera
        );

    private:
        void draw_selected(Scene& scene, GridOptions& grid_options);
        void draw_scene_meaning(Scene& scene);
        void draw_properties(Scene& scene, GameObjectManager& objects);
        void draw_computed(GameObjectManager& objects, const Camera& camera);
        void draw_flags(Scene& scene, GameObjectManager& objects);

        void sync_rename_buffer(const GameObject* selected);
        void capture_object_edit_start(const GameObject& before);
        void commit_object_edit(Scene& scene, EditorCommandType type, const GameObject& after);

        [[nodiscard]] static bool command_object_changed(
            EditorCommandType type,
            const GameObject& before,
            const GameObject& after
        ) noexcept;

        std::optional<GameObjectId> m_rename_target_id;
        std::optional<GameObject> m_pending_object_edit;
		std::array<char, 128> m_rename_buffer{};
    };
}