#pragma once

#include <string>
#include <string_view>

#include "prune/scene/scene_state.hpp"
#include "prune/scene/simple_shooter_state.hpp"

namespace prune {

    class SceneSerializer {
    public:
        [[nodiscard]] static bool save_to_file(
            const SceneState& state,
            const SimpleShooterState& shooter_state,
            std::string_view path,
            std::string& error
        );

        [[nodiscard]] static bool load_from_file(
            SceneState& state,
            SimpleShooterState& shooter_state,
            std::string_view path,
            std::string& error
        );
    };
}
