#pragma once

#include <string_view>

namespace prune {

    constexpr int k_default_object_size = 16;

    constexpr int k_min_object_size = 1;
    constexpr int k_max_object_size = 256;

	constexpr std::string_view k_default_sprite_key = "tank-red";
	constexpr std::string_view k_default_player_sprite_key = "tank-green";

    constexpr std::string_view k_default_simple_shooter_scene_file_path = "simple_shooter_scene.yml";
    constexpr std::string_view k_default_platformer_scene_file_path = "platformer_scene.yml";

    constexpr std::string_view k_default_scene_file_path = k_default_simple_shooter_scene_file_path;

}
