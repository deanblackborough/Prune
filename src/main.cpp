#include "prune/core/app.hpp"
#include "prune/core/app_config.hpp"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <exception>

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    try {
        prune::AppConfig config;
        config.window.title = "Prune Engine";
        config.window.width = 1280;
        config.window.height = 720;
        config.window.vsync = true;
        config.fixed_timestep = 1.0f / 60.0f;

        prune::App app(config);
        app.run();

        return 0;
    }
    catch (const std::exception& e) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Fatal error: %s", e.what());
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "Fatal Error",
            e.what(),
            nullptr
        );
        return 1;
    }
}