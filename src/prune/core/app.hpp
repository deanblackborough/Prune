#pragma once

#include "app_config.hpp"
#include <memory>

namespace prune {

    class Window;
    class Time;
    class Input;
    class ImGuiLayer;

    class App {
    public:
        explicit App(const AppConfig& config);
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;
        App(App&&) = delete;
        App& operator=(App&&) = delete;

        void run();
        void quit() { m_running = false; }

    private:
        void init_sdl();
        void init_opengl();
        void shutdown();

        void update(float dt);
        void fixed_update(float fixed_dt);
        void render();

        std::unique_ptr<Window> m_window;
        std::unique_ptr<Time> m_time;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<ImGuiLayer> m_imgui_layer;

        float m_fixed_timestep;
        bool m_running;
    };

} // namespace prune