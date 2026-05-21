#pragma once

#include "prune/scene/game_object.hpp"

namespace prune::simple_shooter_concepts {

    enum class ObjectKind {
        Player,
        Enemy,
        Projectile,
        Wall,
        Spawn,
        SceneObject
    };

    [[nodiscard]] ObjectKind kind_for(const GameObject& object) noexcept;

    [[nodiscard]] bool is_player(const GameObject& object) noexcept;
    [[nodiscard]] bool is_enemy(const GameObject& object) noexcept;
    [[nodiscard]] bool is_projectile(const GameObject& object) noexcept;
    [[nodiscard]] bool is_wall(const GameObject& object) noexcept;
    [[nodiscard]] bool is_spawn(const GameObject& object) noexcept;

    [[nodiscard]] const char* label(ObjectKind kind) noexcept;
    [[nodiscard]] const char* purpose(ObjectKind kind) noexcept;
    [[nodiscard]] const char* collision_rule(ObjectKind kind) noexcept;

}
