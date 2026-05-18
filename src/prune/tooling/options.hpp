#pragma once

#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class Options {
    public:
        // grid and camera may be nullptr for scene types that don't have them;
        // those sections are simply skipped.
        void draw(SceneOptions& scene_options, GridOptions* grid_options, SceneCamera* camera);
    private:

    };
}