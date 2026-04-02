#pragma once

#include <SDL2/SDL.h>

namespace prune {

    class Scene {
    public:
        virtual ~Scene() = default;

        virtual void on_enter() {}
        virtual void on_exit() {}

        virtual void handle_event(const SDL_Event& event) = 0;
        virtual void update(float dt) = 0;
        virtual void render(SDL_Renderer* renderer) = 0;
    };

}