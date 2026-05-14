#pragma once

#include "prune/scene/game_object.hpp"

namespace prune::simple_shooter_factory {

    [[nodiscard]] GameObject create_player();
    [[nodiscard]] GameObject create_initial_block();
    [[nodiscard]] GameObject create_enemy();
    [[nodiscard]] GameObject create_bullet_from_player(
        const GameObject& player,
        float bullet_speed,
        float bullet_lifetime
    );

}
