#pragma once

#include "app_config.hpp"
#include <SDL2/SDL.h>
#include <cstdint>

namespace prune {

    class Window {
    public:
        explicit Window(const WindowConfig& config);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;

        bool poll_events();
        void swap_buffers();

        uint32_t width() const { return m_width; }
        uint32_t height() const { return m_height; }
        float aspect_ratio() const;

        SDL_Window* native_handle() const { return m_window; }
        SDL_GLContext gl_context() const { return m_gl_context; }

        bool should_close() const { return m_should_close; }

    private:
        void create_window(const WindowConfig& config);
        void create_gl_context();

        SDL_Window* m_window;
        SDL_GLContext m_gl_context;
        uint32_t m_width;
        uint32_t m_height;
        bool m_should_close;
    };

} // namespace prune