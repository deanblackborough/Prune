#pragma once

#include <SDL2/SDL.h>

namespace prune {

    class Input;

    class Scene {
    public:
        virtual ~Scene() = default;

        virtual void on_enter() {}
        virtual void on_exit() {}

        virtual void update(float dt, const Input& input) = 0;
        virtual void render(SDL_Renderer* renderer) = 0;
    };

}