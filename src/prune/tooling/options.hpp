#pragma once

#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class Options {
    public:
        void draw(SceneOptions& scene_options, GridOptions& grid_options, SceneCamera& camera);
    private:

    };
}