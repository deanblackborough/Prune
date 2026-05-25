#include "prune/scene/scene_factory.hpp"

#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer/platformer_scene.hpp"
#include "prune/scene/simple_shooter/simple_shooter_scene.hpp"

namespace prune {

    const SceneDescriptor* scene_descriptor_for(SceneType type) noexcept
    {
        for (const SceneDescriptor& descriptor : k_scene_descriptors) {
            if (descriptor.type == type) {
                return &descriptor;
            }
        }

        return nullptr;
    }

    const SceneDescriptor* scene_descriptor_for_id(std::string_view id) noexcept
    {
        for (const SceneDescriptor& descriptor : k_scene_descriptors) {
            if (descriptor.id == id) {
                return &descriptor;
            }
        }

        return nullptr;
    }

    std::unique_ptr<Scene> SceneFactory::create(
        SceneType type,
        int window_width,
        int window_height
    )
    {
        switch (type) {
        case SceneType::Platformer:
            return std::make_unique<PlatformerScene>(window_width, window_height);

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
            const SceneDescriptor* descriptor = scene_descriptor_for_id(scene_type);

            if (!descriptor) {
                error = "Unknown scene_type: " + scene_type;
                return nullptr;
            }

            std::unique_ptr<Scene> scene = create(descriptor->type, window_width, window_height);

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
