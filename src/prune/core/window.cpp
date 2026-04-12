#include "prune/core/window.hpp"
#include <stdexcept>

namespace prune {

    Window::Window(const WindowConfig& config)
        : m_width(config.width)
        , m_height(config.height)
    {
        m_window = SDL_CreateWindow(
            config.title.c_str(),
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            config.width,
            config.height,
            SDL_WINDOW_SHOWN
        );

        if (!m_window) {
            throw std::runtime_error(std::string("Failed to create SDL window: ") + SDL_GetError());
        }

        m_renderer = SDL_CreateRenderer(
            m_window,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if (!m_renderer) {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
            throw std::runtime_error(std::string("Failed to create SDL renderer: ") + SDL_GetError());
        }
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