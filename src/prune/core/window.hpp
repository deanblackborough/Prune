#pragma once

#include <SDL2/SDL.h>
#include <string>

namespace prune {

    struct WindowConfig {
        std::string title = "Prune";
        int width = 1280;
        int height = 720;
    };

    class Window {
    public:
        explicit Window(const WindowConfig& config);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        SDL_Window* sdl_window() const { return m_window; }
        SDL_Renderer* renderer() const { return m_renderer; }

        int width() const { return m_width; }
        int height() const { return m_height; }

    private:
        SDL_Window* m_window = nullptr;
        SDL_Renderer* m_renderer = nullptr;
        int m_width = 0;
        int m_height = 0;
    };

}