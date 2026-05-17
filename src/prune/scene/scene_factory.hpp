#pragma once

#include <memory>
#include <string_view>

#include "prune/scene/scene.hpp"

namespace prune {

    enum class SceneType {
        SimpleShooter,
        Platformer
    };

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
