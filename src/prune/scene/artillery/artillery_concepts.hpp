#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/object_concept.hpp"

namespace prune::artillery_concepts {

    enum class ObjectKind {
        Unknown = 0,
        Tank,
        TerrainLine,
        Projectile
    };

    [[nodiscard]] ObjectKind kind_for(const GameObject& object) noexcept;
    [[nodiscard]] bool is_tank(const GameObject& object) noexcept;
    [[nodiscard]] bool is_terrain_line(const GameObject& object) noexcept;
    [[nodiscard]] bool is_projectile(const GameObject& object) noexcept;
    [[nodiscard]] ObjectConcept describe_object(const GameObject& object) noexcept;

}
