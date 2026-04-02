#pragma once

#include "window.hpp"

namespace prune {

    struct AppConfig {
        WindowConfig window;
        float fixed_timestep = 1.0f / 60.0f;
    };

} // namespace prune