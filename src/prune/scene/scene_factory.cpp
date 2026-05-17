#include "prune/scene/scene_factory.hpp"

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/simple_shooter_scene.hpp"

namespace prune {

    std::unique_ptr<Scene> SceneFactory::create(
        SceneType type,
        int window_width,
        int window_height
    )
    {
        switch (type) {
        case SceneType::SimpleShooter:
        default:
            return std::make_unique<SimpleShooterScene>(window_width, window_height);
        }
    }

    std::unique_ptr<Scene> SceneFactory::create_from_file(
        std::string_view path,
        int window_width,
        int window_height,
        std::string& error
    )
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            if (!root["scene_type"]) {
                error = "Save file is missing scene_type.";
                return nullptr;
            }

            const std::string scene_type = root["scene_type"].as<std::string>();

            std::unique_ptr<Scene> scene;

            if (scene_type == "simple_shooter") {
                scene = create(SceneType::SimpleShooter, window_width, window_height);
            }
            else {
                error = "Unknown scene_type: " + scene_type;
                return nullptr;
            }

            if (!scene->load_from_file(path, error)) {
                return nullptr;
            }

            return scene;
        }
        catch (const YAML::Exception& ex) {
            error = ex.what();
            return nullptr;
        }
        catch (const std::exception& ex) {
            error = ex.what();
            return nullptr;
        }
    }
}
