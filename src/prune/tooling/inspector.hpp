#pragma once

#include <array>
#include <optional>

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/simple_shooter_scene.hpp"
#include "prune/scene/player_controller.hpp"

namespace prune {

    class Inspector {
    public:
        void draw(
            GameObjectManager& objects,
            GridOptions& grid_options,
            const Camera& camera
        );

    private:
        void draw_selected(GameObjectManager& objects, GridOptions& grid_options);
        void draw_properties(GameObjectManager& objects);
        void draw_computed(GameObjectManager& objects, const Camera& camera);
        void draw_flags(GameObjectManager& objects);
        [[nodiscard]] bool is_selected_player(const GameObject* selected, GameObjectId player_id) const noexcept;

        void sync_rename_buffer(const GameObject* selected);


        std::optional<GameObjectId> m_rename_target_id;
		std::array<char, 128> m_rename_buffer{};
    };
}