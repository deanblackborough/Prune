#pragma once

#include <string_view>

namespace prune {

    constexpr int k_default_object_size = 16;

    constexpr int k_min_object_size = 1;
    constexpr int k_max_object_size = 256;

	constexpr std::string_view k_default_sprite_key = "tank-red";
	constexpr std::string_view k_default_player_sprite_key = "tank-green";

}
