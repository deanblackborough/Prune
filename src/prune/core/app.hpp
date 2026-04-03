#pragma once

#include "input.hpp"
#include "window.hpp"

#include <SDL2/SDL.h>
#include <memory>

namespace prune {

    class Time;
    class Scene;

    struct AppConfig {
        WindowConfig window{};
        float fixed_timestep = 1.0f / 60.0f;
    };

    class App {
    public:
        explicit App(const AppConfig& config);
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        void run();

        [[nodiscard]] Input& input() noexcept { return *m_input; }
        [[nodiscard]] const Input& input() const noexcept { return *m_input; }

    private:
        static void init_sdl();
        static void shutdown_sdl();

        void process_events();
        void handle_event(const SDL_Event& event);
        void update(float dt);
        void render();

        void init_imgui();
        void shutdown_imgui();
        void begin_imgui_frame();

        std::unique_ptr<Window> m_window;
        std::unique_ptr<Time> m_time;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<Scene> m_scene;

        float m_fixed_timestep = 1.0f / 60.0f;
        float m_accumulator = 0.0f;
        bool m_running = false;
    };

}