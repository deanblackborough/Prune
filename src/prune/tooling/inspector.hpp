#pragma once

#include "../scene/game_object.hpp"
#include "../scene/game_object_manager.hpp"
#include "../scene/player_controller.hpp"

namespace prune {

    class Inspector {
    public:
        void draw(GameObjectManager& objects, GameObjectId player_id, PlayerController& player_controller);

    private:
        void draw_selected(GameObjectManager& objects, GameObjectId player_id);
        void draw_actions(GameObjectManager& objects, GameObjectId player_id);
        void draw_properties(GameObjectManager& objects, GameObjectId player_id, PlayerController& player_controller);
        void draw_computed(GameObjectManager& objects);
        void draw_flags(GameObjectManager& objects, GameObjectId player_id);
    };
}