#include "prune/resources/sprites.hpp"

namespace prune {

    const std::vector<Sprites>& sprite_resources()
    {
        static const std::vector<Sprites> resources{
            { "tank-green", "assets/sprites/tank-green.png", "Tanks" },
            { "tank-blue", "assets/sprites/tank-blue.png", "Tanks" },
            { "tank-red", "assets/sprites/tank-red.png", "Tanks" },

            { "player-1-bit", "assets/sprites/player-1-bit.png", "Characters" },

            { "missing", "assets/sprites/missing.png", "Debug" }
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
