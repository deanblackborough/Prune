#pragma once

#include "input.hpp"
#include "window.hpp"

#include <SDL2/SDL.h>
#include <memory>

namespace prune {

    class Time;
    class Scene;
    class EditorUI;

    struct AppConfig {
        WindowConfig window{};
        float fixed_timestep = 1.0f / 60.0f;
    };

    class App {
    public:

        /**
         * Creates the application and initialises its core systems.
         *
         * This sets up SDL, creates the window, initialise input and timing,
         * prepares Dear ImGui, and creates the initial scene.
         *
         * @param config Application startup configuration, including window
         *               settings and fixed timestep values.
         */
        explicit App(const AppConfig& config);

        /**
         * Shuts down the application and releases owned resources.
         *
         * This is responsible for tearing down the active scene, shutting down
         * Dear ImGui, destroying owned systems, and cleaning up SDL state.
         */
        ~App();

        App(const App&) = delete;
        App& operator=(const App&) = delete;

        /**
         * Starts and runs the main application loop.
         *
         * The loop continues until the application is no longer running. Each
         * iteration processes events, advances time, updates the active scene,
         * and renders the current frame.
         */
        void run();

        /**
         * Returns mutable access to the application's input system.
         *
         * @return Reference to the input system.
         */
        [[nodiscard]] Input& input() noexcept { return *m_input; }

        /**
         * Returns read-only access to the application's input system.
         *
         * @return Const reference to the input system.
         */
        [[nodiscard]] const Input& input() const noexcept { return *m_input; }

    private:

        /**
         * Initialises SDL subsystems required by the application.
         *
         * This should be called before creating the window or using other SDL
         * functionality.
         */
        static void init_sdl();

        /**
         * Shuts down SDL and releases global SDL resources.
         *
         * This should be called during application shutdown after SDL-dependent
         * resources have been destroyed.
         */
        static void shutdown_sdl();

        /**
         * Processes all pending SDL events for the current frame.
         *
         * Events are first forwarded to Dear ImGui so UI can capture input when
         * appropriate. Events are then passed to the application and input system,
         * with care taken to avoid a stale input state.
         */
        void process_events();

        /**
         * Handles application-level SDL events.
         *
         * This is intended for events that affect the application itself, such as
         * quit requests or top-level window events.
         *
         * @param event The SDL event to handle.
         */
        void handle_event(const SDL_Event& event);

        /**
         * Updates the application state for a fixed simulation step.
         *
         * This forwards update logic to the active scene using the supplied delta
         * time.
         *
         * @param dt Delta time in seconds for the current fixed update step.
         */
        void update(float dt);

        /**
         * Renders the current application frame.
         *
         * This clears the renderer, renders the active scene, renders Dear ImGui,
         * and presents the completed frame to the window.
         */
        void render();

        /**
         * Initialises Dear ImGui for the current SDL window and renderer.
         *
         * This creates the ImGui context, applies base styling, and initialises
         * the SDL2 and SDL renderer backends.
         */
        void init_imgui();

        /**
         * Shuts down Dear ImGui and releases its resources.
         *
         * This shuts down the active ImGui backends and destroys the ImGui
         * context.
         */
        void shutdown_imgui();

        /**
         * Begins a new Dear ImGui frame.
         *
         * This prepares both the SDL2 and SDL renderer backends and starts a new
         * ImGui frame before any UI is built.
         */
        void begin_imgui_frame();

        std::unique_ptr<Window> m_window;
        std::unique_ptr<Time> m_time;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<Scene> m_scene;
        std::unique_ptr<EditorUI> m_editor_ui;

        float m_fixed_timestep = 1.0f / 60.0f;
        float m_accumulator = 0.0f;
        bool m_running = false;
    };

}