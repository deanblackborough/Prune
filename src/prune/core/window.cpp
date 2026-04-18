#include "prune/core/window.hpp"
#include <stdexcept>

namespace prune {

    Window::Window(const WindowConfig& config)
        : m_width(config.width)
        , m_height(config.height)
    {
		Uint32 window_flags = SDL_WINDOW_SHOWN;

        if (config.resizable) {
            window_flags |= SDL_WINDOW_RESIZABLE;
		}

        if (config.fullscreen) {
            window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }

        m_window = SDL_CreateWindow(
            config.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config.width,
            config.height,
            window_flags
        );

        if (!m_window) {
            throw std::runtime_error(std::string("Failed to create SDL window: ") + SDL_GetError());
        }

        Uint32 renderer_flags = SDL_RENDERER_ACCELERATED;

        if (config.vsync) {
            renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
        }

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            renderer_flags
        );

        if (!m_renderer) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
        }

		refresh_size();
    }

    void Window::refresh_size()
    {
        if (!m_window) {
            m_width = 0;
            m_height = 0;
            return;
        }

		if (m_renderer && SDL_GetRendererOutputSize(m_renderer, &m_width, &m_height) == 0) {
            return;
        }

		SDL_GetWindowSize(m_window, &m_width, &m_height);
    }

    Window::~Window()
    {
        if (m_renderer) {
            SDL_DestroyRenderer(m_renderer);
            m_renderer = nullptr;
        }

        if (m_window) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }
    }
}