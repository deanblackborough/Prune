#include <string>

#include "prune/core/defaults.hpp"
#include "prune/scene/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter_ids.hpp"

namespace prune::simple_shooter_factory {

    GameObject create_player()
    {
        GameObject player;
        player.identity.name = "Player";
        player.identity.type = GameObjectType::Object;
        player.runtime.behaviour = simple_shooter_ids::player_behaviour;
        player.editor.renameable = false;
        player.editor.movable = false;
        player.editor.deletable = false;
        player.editor.cloneable = false;
        player.size.width = k_default_object_size;
        player.size.height = k_default_object_size;
        player.render.type = RenderType::Sprite;
        player.render.sprite.sprite_key = std::string(k_default_player_sprite_key);
        player.render.rectangle.color[0] = 0.3f;
        player.render.rectangle.color[1] = 0.8f;
        player.render.rectangle.color[2] = 0.5f;
        player.transform.x = 128.0f;
        player.transform.y = 128.0f;
        player.lifecycle.active = true;
        player.render.visible = true;
        player.collision.solid = false;

        return player;
    }

    GameObject create_wall()
    {
        GameObject wall;
        wall.identity.name = "Wall";
        wall.identity.type = GameObjectType::Object;
        wall.runtime.behaviour = simple_shooter_ids::wall_behaviour;
        wall.transform.x = 176.0f;
        wall.transform.y = 128.0f;
        wall.size.width = k_default_object_size;
        wall.size.height = k_default_object_size;
        wall.render.type = RenderType::Rectangle;
        wall.render.rectangle.color[0] = 0.8f;
        wall.render.rectangle.color[1] = 0.5f;
        wall.render.rectangle.color[2] = 0.2f;
        wall.lifecycle.active = true;
        wall.render.visible = true;
        wall.collision.solid = true;

        return wall;
    }

    GameObject create_enemy()
    {
        GameObject enemy;
        enemy.identity.name = "Enemy";
        enemy.identity.type = GameObjectType::Object;
        enemy.runtime.behaviour = simple_shooter_ids::enemy_behaviour;
        enemy.transform.x = 256.0f;
        enemy.transform.y = 128.0f;
        enemy.size.width = k_default_object_size;
        enemy.size.height = k_default_object_size;
        enemy.render.type = RenderType::Sprite;
        enemy.render.sprite.sprite_key = "tank-red";
        enemy.render.rectangle.color[0] = 0.9f;
        enemy.render.rectangle.color[1] = 0.2f;
        enemy.render.rectangle.color[2] = 0.2f;
        enemy.lifecycle.active = true;
        enemy.render.visible = true;
        enemy.collision.solid = false;

        return enemy;
    }

    GameObject create_enemy_spawn()
    {
        GameObject spawn;
        spawn.identity.name = "Enemy Spawn";
        spawn.identity.type = GameObjectType::Object;
        spawn.runtime.behaviour = simple_shooter_ids::spawn_behaviour;
        spawn.editor.renameable = false;
        spawn.editor.deletable = false;
        spawn.editor.cloneable = false;
        spawn.transform.x = 256.0f;
        spawn.transform.y = 128.0f;
        spawn.size.width = k_default_object_size;
        spawn.size.height = k_default_object_size;
        spawn.render.type = RenderType::Rectangle;
        spawn.render.rectangle.color[0] = 0.25f;
        spawn.render.rectangle.color[1] = 0.35f;
        spawn.render.rectangle.color[2] = 0.9f;
        spawn.lifecycle.active = true;
        spawn.render.visible = true;
        spawn.collision.solid = false;

        return spawn;
    }

    GameObject create_projectile_from_player(
        const GameObject& player,
        float projectile_speed,
        float projectile_lifetime
    )
    {
        GameObject projectile;
        projectile.identity.name = "Projectile";
        projectile.identity.type = GameObjectType::Runtime;
        projectile.runtime.behaviour = simple_shooter_ids::projectile_behaviour;
        projectile.runtime.persistent = false;

        projectile.editor.selectable = false;
        projectile.editor.renameable = false;
        projectile.editor.movable = false;
        projectile.editor.deletable = false;
        projectile.editor.cloneable = false;

        projectile.size.width = 4;
        projectile.size.height = 4;
        projectile.render.type = RenderType::Rectangle;
        projectile.render.rectangle.color[0] = 0.95f;
        projectile.render.rectangle.color[1] = 0.9f;
        projectile.render.rectangle.color[2] = 0.35f;
        projectile.collision.solid = false;
        projectile.lifecycle.active = true;
        projectile.render.visible = true;
        projectile.motion.facing = player.motion.facing;
        projectile.lifecycle.remaining = projectile_lifetime;

        const float player_center_x = player.transform.x + (static_cast<float>(player.size.width) * 0.5f);
        const float player_center_y = player.transform.y + (static_cast<float>(player.size.height) * 0.5f);

        projectile.transform.x = player_center_x - (static_cast<float>(projectile.size.width) * 0.5f);
        projectile.transform.y = player_center_y - (static_cast<float>(projectile.size.height) * 0.5f);

        switch (player.motion.facing) {
        case Direction::Up:
            projectile.motion.velocity.y = -projectile_speed;
            break;
        case Direction::Down:
            projectile.motion.velocity.y = projectile_speed;
            break;
        case Direction::Left:
            projectile.motion.velocity.x = -projectile_speed;
            break;
        case Direction::Right:
            projectile.motion.velocity.x = projectile_speed;
            break;
        }

        return projectile;
    }
}
