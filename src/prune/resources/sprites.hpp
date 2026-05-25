#pragma once

#include <string_view>
#include <vector>

namespace prune {

    struct Sprites {
        std::string_view key;
        std::string_view path;
        std::string_view category;
    };

    [[nodiscard]] const std::vector<Sprites>& sprite_resources();
    [[nodiscard]] const Sprites* find_sprite_resource(std::string_view key);

}