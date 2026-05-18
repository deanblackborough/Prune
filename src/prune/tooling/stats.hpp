#pragma once

#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class SceneCamera;

    class Stats {
    public:
        // camera may be nullptr for scene types that don't have a camera;
        // the camera stats section is skipped in that case.
        void draw(
            GameObjectManager& objects,
            const SceneViewport& viewport,
            const SceneCamera* camera
        );
    private:

    };
}