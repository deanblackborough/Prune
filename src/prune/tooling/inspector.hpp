#pragma once

#include <array>
#include <optional>

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    struct Camera;

    class Inspector {
    public:
        // grid_options and camera may be nullptr for scene types that lack them;
        // those Inspector sections are gracefully skipped.
        void draw(
            GameObjectManager& objects,
            GridOptions* grid_options,
            const Camera* camera
        );

    private:
        void draw_selected(GameObjectManager& objects, GridOptions* grid_options);
        void draw_properties(GameObjectManager& objects);
        void draw_computed(GameObjectManager& objects, const Camera* camera);
        void draw_flags(GameObjectManager& objects);

        void sync_rename_buffer(const GameObject* selected);


        std::optional<GameObjectId> m_rename_target_id;
		std::array<char, 128> m_rename_buffer{};
    };
}