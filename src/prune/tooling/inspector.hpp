#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/sandbox_scene.hpp"
#include "prune/scene/player_controller.hpp"

namespace prune {

    class Inspector {
    public:
        void draw(
            GameObjectManager& objects,
            GameObjectId player_id,
            PlayerController& player_controller,
            GridOptions& grid_options
        );

    private:
        void draw_selected(GameObjectManager& objects, GameObjectId player_id);
        void draw_actions(GameObjectManager& objects, GameObjectId player_id);
        void draw_properties(
            GameObjectManager& objects,
            GameObjectId player_id,
            PlayerController& player_controller,
            GridOptions& grid_options
        );
        void draw_computed(GameObjectManager& objects);
        void draw_flags(GameObjectManager& objects, GameObjectId player_id);
    };
}