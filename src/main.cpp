#define SDL_MAIN_HANDLED
#include "prune/app/app.hpp"

#include <exception>
#include <iostream>

int main()
{
    try {
        prune::AppConfig config;
        config.window.title = "Prune";

        prune::App app(config);
        app.run();
    }
    catch (const std::exception& exception) {
        std::cerr << exception.what() << '\n';
        return 1;
    }

    return 0;
}