#pragma once

#include <string_view>

#include "prune/scene/game_object.hpp"

namespace prune {

    struct ObjectConcept {
        std::string_view id;
        std::string_view label;
        std::string_view purpose;
        std::string_view collision_rule;
        bool runtime_only = false;
        bool selectable = true;
        bool editable = true;
    };

    namespace object_concepts {

        inline constexpr ObjectConcept scene_object{
            "scene.object",
            "Object",
            "An authored scene object with no additional scene-specific meaning.",
            "Uses the object's generic collision settings.",
            false,
            true,
            true
        };

        inline constexpr ObjectConcept runtime_object{
            "runtime.object",
            "Runtime Object",
            "An object created by gameplay code while the scene is running.",
            "Depends on the scene-specific runtime behaviour.",
            true,
            false,
            false
        };

        [[nodiscard]] inline constexpr ObjectConcept fallback_for(const GameObject& object) noexcept
        {
            return object.identity.type == GameObjectType::Runtime
                ? runtime_object
                : scene_object;
        }

    }

}
