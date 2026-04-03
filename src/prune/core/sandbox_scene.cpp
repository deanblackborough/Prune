#include "sandbox_scene.hpp"
#include <algorithm>
#include "input.hpp"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        constexpr float speed = 250.0f;

        if (input.is_key_down(SDL_SCANCODE_LEFT) || input.is_key_down(SDL_SCANCODE_A)) {
            m_player.x -= speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_RIGHT) || input.is_key_down(SDL_SCANCODE_D)) {
            m_player.x += speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_UP) || input.is_key_down(SDL_SCANCODE_W)) {
            m_player.y -= speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_DOWN) || input.is_key_down(SDL_SCANCODE_S)) {
            m_player.y += speed * dt;
        }
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        SDL_Rect rect{
            static_cast<int>(m_player.x),
            static_cast<int>(m_player.y),
            m_player.size,
            m_player.size
        };

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

}
