#pragma once

#include <cstdint>

namespace prune {

    struct WindowConfig {
        const char* title = "Prune Engine";
        uint32_t width = 1280;
        uint32_t height = 720;
        bool fullscreen = false;
        bool resizable = true;
        bool vsync = true;
    };

    struct AppConfig {
        WindowConfig window;
        float fixed_timestep = 1.0f / 60.0f;
    };

} // namespace prune