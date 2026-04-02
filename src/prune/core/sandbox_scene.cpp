#include "sandbox_scene.hpp"

#include <algorithm>

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::handle_event(const SDL_Event& event)
    {
        if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_a:
                    m_left = true;
                    break;

                case SDLK_RIGHT:
                case SDLK_d:
                    m_right = true;
                    break;

                case SDLK_UP:
                case SDLK_w:
                    m_up = true;
                    break;

                case SDLK_DOWN:
                case SDLK_s:
                    m_down = true;
                    break;

                default:
                    break;
            }
        }

        if (event.type == SDL_KEYUP && !event.key.repeat) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_a:
                    m_left = false;
                    break;

                case SDLK_RIGHT:
                case SDLK_d:
                    m_right = false;
                    break;

                case SDLK_UP:
                case SDLK_w:
                    m_up = false;
                    break;

                case SDLK_DOWN:
                case SDLK_s:
                    m_down = false;
                    break;

                default:
                    break;
            }
        }
    }

    void SandboxScene::update(float dt)
    {
        float move_x = 0.0f;
        float move_y = 0.0f;

        if (m_left) {
            move_x -= 1.0f;
        }
        if (m_right) {
            move_x += 1.0f;
        }
        if (m_up) {
            move_y -= 1.0f;
        }
        if (m_down) {
            move_y += 1.0f;
        }

        m_player.x += move_x * m_player.speed * dt;
        m_player.y += move_y * m_player.speed * dt;

        m_player.x = std::clamp(m_player.x, 0.0f, static_cast<float>(m_window_width - m_player.size));
        m_player.y = std::clamp(m_player.y, 0.0f, static_cast<float>(m_window_height - m_player.size));
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