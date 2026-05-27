#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune::collision {

    [[nodiscard]] bool is_overlapping(const GameObject& a, const GameObject& b) noexcept;
    [[nodiscard]] bool are_active_overlapping(const GameObject& a, const GameObject& b) noexcept;

    void resolve_against_solids(GameObject& object, const GameObjectManager& objects);
}
