#include "window.hpp"
#include <SDL2/SDL.h>
#include <SDL_opengl.h>
#include <stdexcept>

namespace prune {

Window::Window(const WindowConfig& config)
    : m_window(nullptr)
    , m_gl_context(nullptr)
    , m_width(config.width)
    , m_height(config.height)
    , m_should_close(false)
{
    create_window(config);
    create_gl_context();

    if (SDL_GL_SetSwapInterval(config.vsync ? 1 : 0) < 0) {
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Failed to set VSync: %s", SDL_GetError());
    }

    SDL_Log("Window created: %ux%u", m_width, m_height);
}

Window::~Window() {
    if (m_gl_context) {
        SDL_GL_DeleteContext(m_gl_context);
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
    }
}

void Window::create_window(const WindowConfig& config) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

    if (config.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (config.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    m_window = SDL_CreateWindow(
        config.title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        static_cast<int>(m_width),
        static_cast<int>(m_height),
        flags
    );

    if (!m_window) {
        throw std::runtime_error(SDL_GetError());
    }
}

void Window::create_gl_context() {
    m_gl_context = SDL_GL_CreateContext(m_window);
    if (!m_gl_context) {
        throw std::runtime_error(SDL_GetError());
    }

    const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    SDL_Log("OpenGL Renderer: %s", renderer);
    SDL_Log("OpenGL Version: %s", version);
}

void Window::swap_buffers() {
    SDL_GL_SwapWindow(m_window);
}

float Window::aspect_ratio() const {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
}

} // namespace prune