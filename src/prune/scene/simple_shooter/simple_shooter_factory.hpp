#pragma once

#include "prune/scene/game_object.hpp"

namespace prune::simple_shooter_factory {

    [[nodiscard]] GameObject create_player();
    [[nodiscard]] GameObject create_wall();
    [[nodiscard]] GameObject create_enemy();
    [[nodiscard]] GameObject create_enemy_spawn();
    [[nodiscard]] GameObject create_projectile_from_player(
        const GameObject& player,
        float projectile_speed,
        float projectile_lifetime
    );

}
