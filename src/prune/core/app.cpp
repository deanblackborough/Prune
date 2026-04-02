#include "app.hpp"

#include "input.hpp"
#include "prune/tooling/imgui_layer.hpp"
#include "time.hpp"
#include "window.hpp"

#include <SDL2/SDL.h>
#include <SDL_opengl.h>

#include <stdexcept>

namespace prune {

App::App(const AppConfig& config)
    : m_fixed_timestep(config.fixed_timestep)
    , m_running(false) {
    init_sdl();

    m_window = std::make_unique<Window>(config.window);
    m_time = std::make_unique<Time>();
    m_input = std::make_unique<Input>();

    init_opengl();

    m_imgui_layer = std::make_unique<ImGuiLayer>(
        m_window->native_handle(),
        m_window->gl_context());

    SDL_Log("App initialized");
}

App::~App() {
    shutdown();
}

void App::init_sdl() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_Log("SDL initialized");
}

void App::init_opengl() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void App::shutdown() {
    m_imgui_layer.reset();
    m_input.reset();
    m_time.reset();
    m_window.reset();

    SDL_Quit();
    SDL_Log("App shutdown complete");
}

void App::run() {
    m_running = true;

    while (m_running) {
        m_input->begin_frame();

        SDL_Event event{};
        while (SDL_PollEvent(&event)) {
            m_imgui_layer->process_event(event);
            m_input->process_event(event);

            if (event.type == SDL_QUIT) {
                m_running = false;
            }
        }

        if (m_input->is_key_pressed(SDL_SCANCODE_ESCAPE)) {
            m_running = false;
        }

        m_time->tick();

        update(m_time->delta_time());

        m_time->accumulator() += m_time->delta_time();
        while (m_time->accumulator() >= m_fixed_timestep) {
            fixed_update(m_fixed_timestep);
            m_time->accumulator() -= m_fixed_timestep;
        }

        render();
        m_window->swap_buffers();
    }
}

void App::update(float dt) {
    (void)dt;
}

void App::fixed_update(float fixed_dt) {
    (void)fixed_dt;
}

void App::render() {
    glViewport(0, 0, m_window->width(), m_window->height());
    glClear(GL_COLOR_BUFFER_BIT);

    m_imgui_layer->begin_frame();
    m_imgui_layer->render_demo();
    m_imgui_layer->end_frame();
}

} // namespace prune