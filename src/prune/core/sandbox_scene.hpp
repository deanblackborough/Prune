#pragma once

#include "scene.hpp"

namespace prune {

    class SandboxScene final : public Scene {
    public:
        SandboxScene(int window_width, int window_height);

        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;

    private:
        struct Player {
            float x = 100.0f;
            float y = 100.0f;
            float speed = 240.0f;
            int size = 50;
        };

        Player m_player;

        int m_window_width = 0;
        int m_window_height = 0;
    };

}