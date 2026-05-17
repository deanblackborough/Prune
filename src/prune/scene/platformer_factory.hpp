#pragma once

#include "prune/scene/game_object.hpp"

namespace prune::platformer_factory {

    [[nodiscard]] GameObject create_player();
    [[nodiscard]] GameObject create_ground(float x, float y, int width, int height, const char* name);
    [[nodiscard]] GameObject create_hazard(float x, float y);

}
