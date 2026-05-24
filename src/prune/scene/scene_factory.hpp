#pragma once

#include <array>
#include <memory>
#include <string_view>

#include "prune/scene/scene.hpp"

namespace prune {

    enum class SceneType {
        SimpleShooter,
        Platformer
    };

    struct SceneDescriptor {
        SceneType type;
        std::string_view id;
        std::string_view label;
        std::string_view default_file_path;
    };

    inline constexpr std::array<SceneDescriptor, 2> k_scene_descriptors{
        SceneDescriptor{ SceneType::SimpleShooter, "simple_shooter", "Simple Shooter", "simple_shooter_scene.yml" },
        SceneDescriptor{ SceneType::Platformer, "platformer", "Platformer", "platformer_scene.yml" }
    };

    [[nodiscard]] const SceneDescriptor* scene_descriptor_for(SceneType type) noexcept;
    [[nodiscard]] const SceneDescriptor* scene_descriptor_for_id(std::string_view id) noexcept;

    class SceneFactory {
    public:
        [[nodiscard]] static std::unique_ptr<Scene> create(
            SceneType type,
            int window_width,
            int window_height
        );

        [[nodiscard]] static std::unique_ptr<Scene> create_from_file(
            std::string_view path,
            int window_width,
            int window_height,
            std::string& error
        );
    };
}
