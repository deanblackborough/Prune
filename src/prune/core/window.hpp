#pragma once

#include <SDL2/SDL.h>
#include <string>

namespace prune {

    struct WindowConfig {
        std::string title = "Prune";
        int width = 1280;
        int height = 720;
        bool fullscreen = false;
        bool resizable = true;
        bool vsync = true;
    };

    class Window {
    public:

        explicit Window(const WindowConfig& config);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        [[nodiscard]] SDL_Window* sdl_window() const { return m_window; }

        [[nodiscard]] SDL_Renderer* renderer() const { return m_renderer; }

        [[nodiscard]] int width() const { return m_width; }
        [[nodiscard]] int height() const { return m_height; }

    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        int m_width = 0;
        int m_height = 0;
    };

}