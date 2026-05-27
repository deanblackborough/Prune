#include "prune/scene/platformer/platformer_concepts.hpp"
#include "prune/scene/platformer/platformer_ids.hpp"

namespace prune::platformer_concepts {

    ObjectKind kind_for(const GameObject& object) noexcept
    {
        if (is_player(object)) {
            return ObjectKind::Player;
        }

        if (is_player_start(object)) {
            return ObjectKind::PlayerStart;
        }

        if (is_platform(object)) {
            return ObjectKind::Platform;
        }

        if (is_hazard(object)) {
            return ObjectKind::Hazard;
        }

        return ObjectKind::SceneObject;
    }

    bool is_player(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == platformer_ids::player_behaviour;
    }

    bool is_player_start(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == platformer_ids::player_start_behaviour;
    }

    bool is_platform(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == platformer_ids::platform_behaviour;
    }

    bool is_hazard(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == platformer_ids::hazard_behaviour;
    }

    const char* label(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "Player";
        case ObjectKind::PlayerStart:
            return "Player Start";
        case ObjectKind::Platform:
            return "Platform";
        case ObjectKind::Hazard:
            return "Hazard";
        case ObjectKind::SceneObject:
        default:
            return "Scene Object";
        }
    }

    const char* purpose(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "The single controlled actor for the Platformer slice. The game camera follows this object.";
        case ObjectKind::PlayerStart:
            return "Authored reset marker used when the player hits a hazard or falls out of the level.";
        case ObjectKind::Platform:
            return "An authored solid surface the player can stand on or collide with.";
        case ObjectKind::Hazard:
            return "An authored trigger that resets the player to the player start marker on contact.";
        case ObjectKind::SceneObject:
        default:
            return "No Platformer concept is assigned.";
        }
    }

    const char* collision_rule(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "Blocked by Platform objects. Hazards reset the player.";
        case ObjectKind::PlayerStart:
            return "Marker only. It does not collide.";
        case ObjectKind::Platform:
            return "Solid. Blocks horizontal movement and provides grounded state when landed on.";
        case ObjectKind::Hazard:
            return "Trigger only. It is not solid, but player contact causes a reset.";
        case ObjectKind::SceneObject:
        default:
            return "No platformer collision rule.";
        }
    }

    ObjectConcept describe(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return {
                "platformer.player",
                "Player",
                "The single controlled actor for the Platformer slice. The game camera follows this object.",
                "Blocked by Platform objects. Hazards reset the player.",
                false,
                true,
                true
            };
        case ObjectKind::PlayerStart:
            return {
                "platformer.player_start",
                "Player Start",
                "Authored reset marker used when the player hits a hazard or falls out of the level.",
                "Marker only. It does not collide.",
                false,
                true,
                true
            };
        case ObjectKind::Platform:
            return {
                "platformer.platform",
                "Platform",
                "An authored solid surface the player can stand on or collide with.",
                "Solid. Blocks horizontal movement and provides grounded state when landed on.",
                false,
                true,
                true
            };
        case ObjectKind::Hazard:
            return {
                "platformer.hazard",
                "Hazard",
                "An authored trigger that resets the player to the player start marker on contact.",
                "Trigger only. It is not solid, but player contact causes a reset.",
                false,
                true,
                true
            };
        case ObjectKind::SceneObject:
        default:
            return object_concepts::scene_object;
        }
    }

    ObjectConcept describe_object(const GameObject& object) noexcept
    {
        ObjectConcept result = describe(kind_for(object));

        if (object.identity.type == GameObjectType::Runtime) {
            result.runtime_only = true;
            result.selectable = false;
            result.editable = false;
        }

        return result;
    }

}
