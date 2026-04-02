#include "app.hpp"
#include "time.hpp"
#include "scene.hpp"
#include "sandbox_scene.hpp"

#include <SDL2/SDL.h>
#include <stdexcept>

namespace prune {

    App::App(const AppConfig& config)
        : m_fixed_timestep(config.fixed_timestep)
    {
        init_sdl();

        m_window = std::make_unique<Window>(config.window);
        m_time = std::make_unique<Time>();

        m_scene = std::make_unique<SandboxScene>(
            m_window->width(),
            m_window->height()
        );
        m_scene->on_enter();
    }

    App::~App()
    {
        if (m_scene) {
            m_scene->on_exit();
        }

        m_scene.reset();
        m_time.reset();
        m_window.reset();

        shutdown_sdl();
    }

    void App::run()
    {
        m_running = true;

        while (m_running) {
            m_time->tick();

            float frame_time = m_time->delta_seconds();

            if (frame_time > 0.25f) {
                frame_time = 0.25f;
            }

            m_accumulator += frame_time;

            process_events();

            while (m_accumulator >= m_fixed_timestep) {
                update(m_fixed_timestep);
                m_accumulator -= m_fixed_timestep;
            }

            render();
        }
    }

    void App::init_sdl()
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
            throw std::runtime_error(std::string("Failed to initialize SDL: ") + SDL_GetError());
        }
    }

    void App::shutdown_sdl()
    {
        SDL_Quit();
    }

    void App::process_events()
    {
        SDL_Event event{};

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                m_running = false;
                return;
            }

            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                m_running = false;
                return;
            }

            if (m_scene) {
                m_scene->handle_event(event);
            }
        }
    }

    void App::update(float dt)
    {
        if (m_scene) {
            m_scene->update(dt);
        }
    }

    void App::render()
    {
        SDL_Renderer* renderer = m_window->renderer();

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        if (m_scene) {
            m_scene->render(renderer);
        }

        SDL_RenderPresent(renderer);
    }

}