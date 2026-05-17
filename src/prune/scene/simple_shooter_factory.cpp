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

    GameObject create_initial_block()
    {
        GameObject block;
        block.identity.name = "Static Block";
        block.identity.type = GameObjectType::Object;
        block.runtime.behaviour = "";
        block.transform.x = 176.0f;
        block.transform.y = 128.0f;
        block.size.width = k_default_object_size;
        block.size.height = k_default_object_size;
        block.render.type = RenderType::Rectangle;
        block.render.rectangle.color[0] = 0.8f;
        block.render.rectangle.color[1] = 0.5f;
        block.render.rectangle.color[2] = 0.2f;
        block.lifecycle.active = true;
        block.collision.solid = true;

        return block;
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

    GameObject create_bullet_from_player(
        const GameObject& player,
        float bullet_speed,
        float bullet_lifetime
    )
    {
        GameObject bullet;
        bullet.identity.name = "Bullet";
        bullet.identity.type = GameObjectType::Runtime;
        bullet.runtime.behaviour = simple_shooter_ids::bullet_behaviour;
        bullet.runtime.persistent = false;

        bullet.editor.selectable = false;
        bullet.editor.renameable = false;
        bullet.editor.movable = false;
        bullet.editor.deletable = false;
        bullet.editor.cloneable = false;

        bullet.size.width = 4;
        bullet.size.height = 4;
        bullet.render.type = RenderType::Rectangle;
        bullet.render.rectangle.color[0] = 0.95f;
        bullet.render.rectangle.color[1] = 0.9f;
        bullet.render.rectangle.color[2] = 0.35f;
        bullet.collision.solid = false;
        bullet.lifecycle.active = true;
        bullet.render.visible = true;
        bullet.motion.facing = player.motion.facing;
        bullet.lifecycle.remaining = bullet_lifetime;

        const float player_center_x = player.transform.x + (static_cast<float>(player.size.width) * 0.5f);
        const float player_center_y = player.transform.y + (static_cast<float>(player.size.height) * 0.5f);

        bullet.transform.x = player_center_x - (static_cast<float>(bullet.size.width) * 0.5f);
        bullet.transform.y = player_center_y - (static_cast<float>(bullet.size.height) * 0.5f);

        switch (player.motion.facing) {
        case Direction::Up:
            bullet.motion.velocity.y = -bullet_speed;
            break;
        case Direction::Down:
            bullet.motion.velocity.y = bullet_speed;
            break;
        case Direction::Left:
            bullet.motion.velocity.x = -bullet_speed;
            break;
        case Direction::Right:
            bullet.motion.velocity.x = bullet_speed;
            break;
        }

        return bullet;
    }
}
