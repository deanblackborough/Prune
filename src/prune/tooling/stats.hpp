#pragma once

#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/game_object.hpp"

namespace prune {

    class Stats {
    public:
        void draw(GameObjectManager& objects, GameObjectId player_id);
    private:

    };
}