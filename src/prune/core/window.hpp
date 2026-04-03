#pragma once

#include <SDL2/SDL.h>
#include <string>

namespace prune {

    /**
     * Configuration used to create an SDL window and renderer.
     *
     * Defines window size, title, and behaviour such as fullscreen,
     * resizability, and vertical sync.
     */
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

        /**
         * Creates a window and associated renderer using the given configuration.
         *
         * Initialises SDL window flags based on the config and creates an
         * SDL_Renderer, optionally enabling vsync.
         *
         * @param config Window configuration settings.
         */
        explicit Window(const WindowConfig& config);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        /**
         * Returns the underlying SDL_Window pointer.
         *
         * Primarily used for integrations such as Dear ImGui.
         *
         * @return Pointer to the SDL window.
         */
        [[nodiscard]] SDL_Window* sdl_window() const { return m_window; }

        /**
         * Returns the SDL_Renderer used for drawing.
         *
         * This is the main rendering interface for the application.
         *
         * @return Pointer to the SDL renderer.
         */
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