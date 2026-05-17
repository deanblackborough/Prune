#include <string>

#include "prune/core/defaults.hpp"
#include "prune/scene/platformer_factory.hpp"
#include "prune/scene/platformer_ids.hpp"

namespace prune::platformer_factory {

    GameObject create_player()
    {
        GameObject player;
        player.identity.name = "Platformer Player";
        player.identity.type = GameObjectType::Object;
        player.runtime.behaviour = platformer_ids::player_behaviour;
        player.editor.renameable = false;
        player.editor.movable = false;
        player.editor.deletable = false;
        player.editor.cloneable = false;
        player.size.width = k_default_object_size;
        player.size.height = k_default_object_size;
        player.render.type = RenderType::Sprite;
        player.render.sprite.sprite_key = std::string(k_default_player_sprite_key);
        player.transform.x = 64.0f;
        player.transform.y = 96.0f;
        player.lifecycle.active = true;
        player.render.visible = true;
        player.collision.solid = false;
        return player;
    }

    GameObject create_ground(float x, float y, int width, int height, const char* name)
    {
        GameObject ground;
        ground.identity.name = name;
        ground.identity.type = GameObjectType::Object;
        ground.runtime.behaviour = platformer_ids::ground_behaviour;
        ground.transform.x = x;
        ground.transform.y = y;
        ground.size.width = width;
        ground.size.height = height;
        ground.render.type = RenderType::Rectangle;
        ground.render.rectangle.color[0] = 0.45f;
        ground.render.rectangle.color[1] = 0.35f;
        ground.render.rectangle.color[2] = 0.55f;
        ground.lifecycle.active = true;
        ground.render.visible = true;
        ground.collision.solid = true;
        return ground;
    }

    GameObject create_hazard(float x, float y)
    {
        GameObject hazard;
        hazard.identity.name = "Hazard";
        hazard.identity.type = GameObjectType::Object;
        hazard.runtime.behaviour = platformer_ids::hazard_behaviour;
        hazard.transform.x = x;
        hazard.transform.y = y;
        hazard.size.width = k_default_object_size;
        hazard.size.height = k_default_object_size;
        hazard.render.type = RenderType::Rectangle;
        hazard.render.rectangle.color[0] = 0.9f;
        hazard.render.rectangle.color[1] = 0.2f;
        hazard.render.rectangle.color[2] = 0.35f;
        hazard.lifecycle.active = true;
        hazard.render.visible = true;
        hazard.collision.solid = false;
        return hazard;
    }

}
