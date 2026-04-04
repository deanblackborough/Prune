#pragma once

#include "scene.hpp"

namespace prune {

    class SandboxScene final : public Scene {
    public:
        SandboxScene(int window_width, int window_height);

        /**
         * Updates the scene state.
         *
         * This is where gameplay or simulation logic should run for the current
         * frame or fixed timestep.
         *
         * @param dt Delta time in seconds for the current update step.
         * @param input Read-only access to the application's input state.
         */
        void update(float dt, const Input& input) override;

        /**
         * Renders the scene using the provided SDL renderer.
         *
         * @param renderer SDL renderer used for scene drawing.
         */
        void render(SDL_Renderer* renderer) override;

        /**
         * Draws scene-specific content inside the shared inspector panel.
         */
        void draw_inspector_ui() override;

        /**
         * Draws scene-specific content inside the shared debug panel.
         */
        void draw_debug_ui() override;

    private:
        struct Player {
            float x = 100.0f;
            float y = 100.0f;
            float speed = 240.0f;
            int size = 50;
            float color[3] = {0.3f, 0.8f, 0.5f};
        };

        void clamp_player_to_window();

        Player m_player;

        int m_window_width = 0;
        int m_window_height = 0;
    };

}