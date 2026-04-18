#pragma once

#include <random>
#include <array>

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune {

    class Outliner {
    public:
        void draw(
            GameObjectManager& objects,
            float camera_x,
            float camera_y,
            int viewport_width,
            int viewport_height,
            bool snap_to_grid,
            int grid_size
        );
    private:
        static std::string object_label(const GameObject& object);
        GameObjectId create_block(GameObjectManager& objects, float x, float y);
        float random_color_component();
        Transform next_block_spawn_position(
            float viewport_width,
            float viewport_height,
            float camera_x,
            float camera_y
        ) const;
        [[nodiscard]] Transform find_spawn_position(
            const GameObjectManager& objects,
            float base_x,
            float base_y,
            bool snap_to_grid,
            int grid_size,
            int object_width,
            int object_height
        ) const;

        [[nodiscard]] bool is_space_free(
            const GameObjectManager& objects,
            float x,
            float y,
            int width,
            int height
        ) const;
        bool contains_case_insensitive(std::string_view text, std::string_view query) const;

        std::array<char, 128> m_object_search{};

        // Random number generation
        std::mt19937 m_rng{std::random_device{}()};
        std::uniform_real_distribution<float> m_color_dist{0.2f, 1.0f};
    };
}