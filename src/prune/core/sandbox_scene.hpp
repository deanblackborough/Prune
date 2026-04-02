#pragma once

#include "scene.hpp"

namespace prune {

    class SandboxScene final : public Scene {
    public:
        SandboxScene(int window_width, int window_height);

        void handle_event(const SDL_Event& event) override;
        void update(float dt) override;
        void render(SDL_Renderer* renderer) override;

    private:
        struct Player {
            float x = 100.0f;
            float y = 100.0f;
            float speed = 240.0f;
            int size = 50;
        };

        Player m_player;

        bool m_left = false;
        bool m_right = false;
        bool m_up = false;
        bool m_down = false;

        int m_window_width = 0;
        int m_window_height = 0;
    };

}