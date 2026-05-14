#include <string>

#include "prune/core/defaults.hpp"
#include "prune/scene/simple_shooter_factory.hpp"

namespace prune::simple_shooter_factory {

    GameObject create_player()
    {
        GameObject player;
        player.name = "Player";
        player.type = GameObjectType::Object;
        player.runtime.behaviour = "simple_shooter.player";
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
        player.active = true;
        player.render.visible = true;
        player.collision.solid = false;

        return player;
    }

    GameObject create_initial_block()
    {
        GameObject block;
        block.name = "Static Block";
        block.type = GameObjectType::Object;
        block.runtime.behaviour = "";
        block.transform.x = 176.0f;
        block.transform.y = 128.0f;
        block.size.width = k_default_object_size;
        block.size.height = k_default_object_size;
        block.render.type = RenderType::Rectangle;
        block.render.rectangle.color[0] = 0.8f;
        block.render.rectangle.color[1] = 0.5f;
        block.render.rectangle.color[2] = 0.2f;
        block.active = true;
        block.collision.solid = true;

        return block;
    }

    GameObject create_enemy()
    {
        GameObject enemy;
        enemy.name = "Enemy";
        enemy.type = GameObjectType::Object;
        enemy.runtime.behaviour = "simple_shooter.enemy";
        enemy.transform.x = 256.0f;
        enemy.transform.y = 128.0f;
        enemy.size.width = k_default_object_size;
        enemy.size.height = k_default_object_size;
        enemy.render.type = RenderType::Sprite;
        enemy.render.sprite.sprite_key = "tank-red";
        enemy.render.rectangle.color[0] = 0.9f;
        enemy.render.rectangle.color[1] = 0.2f;
        enemy.render.rectangle.color[2] = 0.2f;
        enemy.active = true;
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
        bullet.name = "Bullet";
        bullet.type = GameObjectType::Runtime;
        bullet.runtime.behaviour = "simple_shooter.bullet";
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
        bullet.active = true;
        bullet.render.visible = true;
        bullet.facing = player.facing;
        bullet.lifetime = bullet_lifetime;

        const float player_center_x = player.transform.x + (static_cast<float>(player.size.width) * 0.5f);
        const float player_center_y = player.transform.y + (static_cast<float>(player.size.height) * 0.5f);

        bullet.transform.x = player_center_x - (static_cast<float>(bullet.size.width) * 0.5f);
        bullet.transform.y = player_center_y - (static_cast<float>(bullet.size.height) * 0.5f);

        switch (player.facing) {
        case Direction::Up:
            bullet.velocity.y = -bullet_speed;
            break;
        case Direction::Down:
            bullet.velocity.y = bullet_speed;
            break;
        case Direction::Left:
            bullet.velocity.x = -bullet_speed;
            break;
        case Direction::Right:
            bullet.velocity.x = bullet_speed;
            break;
        }

        return bullet;
    }
}
