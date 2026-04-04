#pragma once

#include <SDL2/SDL.h>

namespace prune {

    class Input;

    class Scene {
    public:
        virtual ~Scene() = default;

        /**
         * Called when the scene becomes active.
         *
         * Override this to perform scene setup, initialise state, or prepare
         * resources required when entering the scene.
         */
        virtual void on_enter() {}

        /**
         * Called when the scene is about to be deactivated or destroyed.
         *
         * Override this to perform scene-specific clean-up before the scene is
         * removed.
         */
        virtual void on_exit() {}

        /**
         * Updates the scene state.
         *
         * This is where gameplay or simulation logic should run for the current
         * frame or fixed timestep.
         *
         * @param dt Delta time in seconds for the current update step.
         * @param input Read-only access to the application's input state.
         */
        virtual void update(float dt, const Input& input) = 0;

        /**
         * Renders the scene using the provided SDL renderer.
         *
         * Implementations should draw all scene visuals for the current frame.
         *
         * @param renderer SDL renderer used for scene drawing.
         */
        virtual void render(SDL_Renderer* renderer) = 0;

        /**
         * Draws scene-specific content for the inspector panel.
         *
         * The tooling layer owns the actual window. Scenes only provide the
         * controls shown inside it.
         */
        virtual void draw_inspector_ui() {}

        /**
         * Draws scene-specific content for the debug panel.
         *
         * The tooling layer owns the actual window. Scenes only provide any
         * scene-level diagnostics shown inside it.
         */
        virtual void draw_debug_ui() {}
    };

}