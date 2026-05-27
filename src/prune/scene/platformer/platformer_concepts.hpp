#pragma once

#include "prune/scene/game_object.hpp"
#include "prune/scene/object_concept.hpp"

namespace prune::platformer_concepts {

    enum class ObjectKind {
        Player,
        PlayerStart,
        Platform,
        Hazard,
        SceneObject
    };

    [[nodiscard]] ObjectKind kind_for(const GameObject& object) noexcept;

    [[nodiscard]] bool is_player(const GameObject& object) noexcept;
    [[nodiscard]] bool is_player_start(const GameObject& object) noexcept;
    [[nodiscard]] bool is_platform(const GameObject& object) noexcept;
    [[nodiscard]] bool is_hazard(const GameObject& object) noexcept;

    [[nodiscard]] const char* label(ObjectKind kind) noexcept;
    [[nodiscard]] const char* purpose(ObjectKind kind) noexcept;
    [[nodiscard]] const char* collision_rule(ObjectKind kind) noexcept;
    [[nodiscard]] ObjectConcept describe(ObjectKind kind) noexcept;
    [[nodiscard]] ObjectConcept describe_object(const GameObject& object) noexcept;

}
