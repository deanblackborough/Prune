#pragma once

#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/sandbox_scene.hpp"

namespace prune {

    class Stats {
    public:
        void draw(
            GameObjectManager& objects,
            GameObjectId player_id,
            int viewport_width,
            int viewport_height,
            const CameraState& cameras
        );
    private:

    };
}