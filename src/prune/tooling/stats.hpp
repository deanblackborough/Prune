#pragma once

#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/scene_camera.hpp"

namespace prune {

    class Stats {
    public:
        void draw(
            GameObjectManager& objects,
            const SceneViewport& viewport,
            const SceneCamera& camera
        );
    private:

    };
}