#pragma once

#include "prune/scene/game_object.hpp"

namespace prune::artillery_factory {

    GameObject create_tank(float x, float y, const char* name, const char* sprite_key);
    GameObject create_terrain_line(float x, float y, int width, int height, const char* name);
    GameObject create_projectile(float x, float y);

}
