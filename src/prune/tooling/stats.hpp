#pragma once

#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/simple_shooter/simple_shooter_scene.hpp"

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