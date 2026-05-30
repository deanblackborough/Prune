#include <string>

#include "prune/core/defaults.hpp"
#include "prune/scene/artillery/artillery_factory.hpp"
#include "prune/scene/artillery/artillery_ids.hpp"

namespace prune::artillery_factory {

    GameObject create_tank(float x, float y, const char* name, const char* sprite_key)
    {
        GameObject tank;
        tank.identity.name = name;
        tank.identity.type = GameObjectType::Authored;
        tank.runtime.behaviour = artillery_ids::tank_behaviour;
        tank.editor.renameable = false;
        tank.editor.deletable = false;
        tank.editor.cloneable = false;
        tank.size.width = k_default_object_size;
        tank.size.height = k_default_object_size;
        tank.render.type = RenderType::Sprite;
        tank.render.sprite.sprite_key = std::string(sprite_key);
        tank.transform.x = x;
        tank.transform.y = y;
        tank.lifecycle.active = true;
        tank.render.visible = true;
        tank.collision.solid = true;
        return tank;
    }

    GameObject create_terrain_line(float x, float y, int width, int height, const char* name)
    {
        GameObject terrain;
        terrain.identity.name = name;
        terrain.identity.type = GameObjectType::Authored;
        terrain.runtime.behaviour = artillery_ids::terrain_behaviour;
        terrain.transform.x = x;
        terrain.transform.y = y;
        terrain.size.width = width;
        terrain.size.height = height;
        terrain.render.type = RenderType::Rectangle;
        terrain.render.rectangle.color[0] = 0.36f;
        terrain.render.rectangle.color[1] = 0.48f;
        terrain.render.rectangle.color[2] = 0.30f;
        terrain.lifecycle.active = true;
        terrain.render.visible = true;
        terrain.collision.solid = true;
        return terrain;
    }

    GameObject create_projectile(float x, float y)
    {
        GameObject projectile;
        projectile.identity.name = "Projectile";
        projectile.identity.type = GameObjectType::Runtime;
        projectile.runtime.behaviour = artillery_ids::projectile_behaviour;
        projectile.runtime.persistent = false;
        projectile.editor.selectable = false;
        projectile.editor.renameable = false;
        projectile.editor.movable = false;
        projectile.editor.deletable = false;
        projectile.editor.cloneable = false;
        projectile.transform.x = x;
        projectile.transform.y = y;
        projectile.size.width = 4;
        projectile.size.height = 4;
        projectile.render.type = RenderType::Rectangle;
        projectile.render.rectangle.color[0] = 1.0f;
        projectile.render.rectangle.color[1] = 0.82f;
        projectile.render.rectangle.color[2] = 0.28f;
        projectile.lifecycle.active = true;
        projectile.render.visible = true;
        projectile.collision.solid = false;
        return projectile;
    }

}
