#include "prune/resources/sprites.hpp"

namespace prune {

    const std::vector<Sprites>& sprite_resources()
    {
        static const std::vector<Sprites> resources{
            { "player", "assets/sprites/player.png" },
            { "crate", "assets/sprites/crate.png" }
        };

        return resources;
    }

    const Sprites* find_sprite_resource(std::string_view key)
    {
        for (const Sprites& resource : sprite_resources()) {
            if (resource.key == key) {
                return &resource;
            }
        }

        return nullptr;
    }

}