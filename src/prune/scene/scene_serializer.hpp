#pragma once

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    // SceneSerializer provides individual save/load pieces that scenes compose
    // to build their own serialization. A scene calls only the pieces it needs:
    // a card-game scene might call save_objects + save_scene_options only,
    // whereas a platformer scene also calls save_camera + save_grid.
    class SceneSerializer {
    public:
        // ---- Save pieces ----

        static void save_objects(const GameObjectManager& objects, YAML::Node& root);
        static void save_scene_options(const SceneOptions& options, YAML::Node& root);
        static void save_camera(const SceneCamera& camera, YAML::Node& root);
        static void save_grid(const GridOptions& grid, YAML::Node& root);

        // ---- Load pieces ----

        [[nodiscard]] static bool load_objects(
            GameObjectManager& objects,
            const YAML::Node& root,
            std::string& error
        );

        [[nodiscard]] static bool load_scene_options(
            SceneOptions& options,
            const YAML::Node& root,
            std::string& error
        );

        [[nodiscard]] static bool load_camera(
            SceneCamera& camera,
            const YAML::Node& root,
            std::string& error
        );

        [[nodiscard]] static bool load_grid(
            GridOptions& grid,
            const YAML::Node& root,
            std::string& error
        );
    };
}
