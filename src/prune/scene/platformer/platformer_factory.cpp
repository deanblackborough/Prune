#include <string>

#include "prune/core/defaults.hpp"
#include "prune/scene/platformer/platformer_factory.hpp"
#include "prune/scene/platformer/platformer_ids.hpp"

namespace prune::platformer_factory {

    GameObject create_player()
    {
        GameObject player;
        player.identity.name = "Platformer Player";
        player.identity.type = GameObjectType::Authored;
        player.runtime.behaviour = platformer_ids::player_behaviour;
        player.editor.renameable = false;
        player.editor.movable = false;
        player.editor.deletable = false;
        player.editor.cloneable = false;
        player.size.width = k_default_object_size;
        player.size.height = k_default_object_size;
        player.render.type = RenderType::Sprite;
        player.render.sprite.sprite_key = std::string(k_default_player_platformer_sprite_key);
        player.transform.x = 32.0f;
        player.transform.y = 112.0f;
        player.lifecycle.active = true;
        player.render.visible = true;
        player.collision.solid = false;
        return player;
    }

    GameObject create_player_start(float x, float y)
    {
        GameObject start;
        start.identity.name = "Player Start";
        start.identity.type = GameObjectType::Authored;
        start.runtime.behaviour = platformer_ids::player_start_behaviour;
        start.transform.x = x;
        start.transform.y = y;
        start.size.width = k_default_object_size;
        start.size.height = k_default_object_size;
        start.render.type = RenderType::Rectangle;
        start.render.rectangle.color[0] = 0.25f;
        start.render.rectangle.color[1] = 0.75f;
        start.render.rectangle.color[2] = 0.95f;
        start.lifecycle.active = true;
        start.render.visible = true;
        start.collision.solid = false;
        return start;
    }

    GameObject create_platform(float x, float y, int width, int height, const char* name)
    {
        GameObject platform;
        platform.identity.name = name;
        platform.identity.type = GameObjectType::Authored;
        platform.runtime.behaviour = platformer_ids::platform_behaviour;
        platform.transform.x = x;
        platform.transform.y = y;
        platform.size.width = width;
        platform.size.height = height;
        platform.render.type = RenderType::Rectangle;
        platform.render.rectangle.color[0] = 0.45f;
        platform.render.rectangle.color[1] = 0.35f;
        platform.render.rectangle.color[2] = 0.55f;
        platform.lifecycle.active = true;
        platform.render.visible = true;
        platform.collision.solid = true;
        return platform;
    }

    GameObject create_hazard(float x, float y)
    {
        return create_hazard(x, y, k_default_object_size, k_default_object_size, "Hazard");
    }

    GameObject create_hazard(float x, float y, int width, int height, const char* name)
    {
        GameObject hazard;
        hazard.identity.name = name;
        hazard.identity.type = GameObjectType::Authored;
        hazard.runtime.behaviour = platformer_ids::hazard_behaviour;
        hazard.transform.x = x;
        hazard.transform.y = y;
        hazard.size.width = width;
        hazard.size.height = height;
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
