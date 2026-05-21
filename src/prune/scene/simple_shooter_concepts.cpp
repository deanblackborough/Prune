#include "prune/scene/simple_shooter_concepts.hpp"

#include "prune/scene/simple_shooter_ids.hpp"

namespace prune::simple_shooter_concepts {

    ObjectKind kind_for(const GameObject& object) noexcept
    {
        if (is_player(object)) {
            return ObjectKind::Player;
        }

        if (is_enemy(object)) {
            return ObjectKind::Enemy;
        }

        if (is_projectile(object)) {
            return ObjectKind::Projectile;
        }

        if (is_wall(object)) {
            return ObjectKind::Wall;
        }

        if (is_spawn(object)) {
            return ObjectKind::Spawn;
        }

        return ObjectKind::SceneObject;
    }

    bool is_player(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == simple_shooter_ids::player_behaviour;
    }

    bool is_enemy(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == simple_shooter_ids::enemy_behaviour;
    }

    bool is_projectile(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == simple_shooter_ids::projectile_behaviour;
    }

    bool is_wall(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == simple_shooter_ids::wall_behaviour;
    }

    bool is_spawn(const GameObject& object) noexcept
    {
        return object.runtime.behaviour == simple_shooter_ids::spawn_behaviour;
    }

    const char* label(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "Player";
        case ObjectKind::Enemy:
            return "Enemy";
        case ObjectKind::Projectile:
            return "Projectile";
        case ObjectKind::Wall:
            return "Wall";
        case ObjectKind::Spawn:
            return "Enemy Spawn";
        case ObjectKind::SceneObject:
        default:
            return "Scene Object";
        }
    }

    const char* purpose(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "The single controlled actor for the Simple Shooter slice.";
        case ObjectKind::Enemy:
            return "The single target that moves toward the player and respawns when hit.";
        case ObjectKind::Projectile:
            return "A runtime projectile fired by the player.";
        case ObjectKind::Wall:
            return "An authored solid obstacle used for player and projectile collision.";
        case ObjectKind::Spawn:
            return "The authored reset position used when the enemy respawns.";
        case ObjectKind::SceneObject:
        default:
            return "No Simple Shooter concept is assigned.";
        }
    }

    const char* collision_rule(ObjectKind kind) noexcept
    {
        switch (kind) {
        case ObjectKind::Player:
            return "Movement is blocked by solid wall objects.";
        case ObjectKind::Enemy:
            return "Projectile hits reset the enemy. Enemy/player contact is not handled in this slice.";
        case ObjectKind::Projectile:
            return "Hits enemies, stops at walls, and expires after its lifetime.";
        case ObjectKind::Wall:
            return "Blocks player movement and stops projectiles.";
        case ObjectKind::Spawn:
            return "Marker only. It does not collide.";
        case ObjectKind::SceneObject:
        default:
            return "No shooter collision rule.";
        }
    }

}
