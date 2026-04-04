#include "sandbox_scene.hpp"
#include <algorithm>

#include "imgui.h"
#include "input.hpp"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        if (input.is_key_down(SDL_SCANCODE_LEFT) || input.is_key_down(SDL_SCANCODE_A)) {
            m_player.x -= m_player.speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_RIGHT) || input.is_key_down(SDL_SCANCODE_D)) {
            m_player.x += m_player.speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_UP) || input.is_key_down(SDL_SCANCODE_W)) {
            m_player.y -= m_player.speed * dt;
        }

        if (input.is_key_down(SDL_SCANCODE_DOWN) || input.is_key_down(SDL_SCANCODE_S)) {
            m_player.y += m_player.speed * dt;
        }

        if (input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            const auto mouse_x = static_cast<float>(input.mouse_x());
            const auto mouse_y = static_cast<float>(input.mouse_y());
            const auto half_size = static_cast<float>(m_player.size) * 0.5f;

            m_player.x = mouse_x - half_size;
            m_player.y = mouse_y - half_size;
        }

        clamp_player_to_window();
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        SDL_Rect rect{
            static_cast<int>(m_player.x),
            static_cast<int>(m_player.y),
            m_player.size,
            m_player.size
        };

        SDL_SetRenderDrawColor(
            renderer,
            static_cast<Uint8>(m_player.color[0] * 255.0f),
            static_cast<Uint8>(m_player.color[1] * 255.0f),
            static_cast<Uint8>(m_player.color[2] * 255.0f),
            255
        );
        SDL_RenderFillRect(renderer, &rect);
    }

    void SandboxScene::draw_inspector_ui() {
        ImGui::TextUnformatted("Player");

        ImGui::Spacing();
        ImGui::TextUnformatted("Position");
        ImGui::Text("X: %.1f", m_player.x);
        ImGui::Text("Y: %.1f", m_player.y);

        ImGui::Separator();

        ImGui::SliderInt("Size", &m_player.size, 10, 200);
        ImGui::SliderFloat("Speed", &m_player.speed, 50.0f, 600.0f, "%.1f");

        ImGui::Separator();

        ImGui::ColorEdit3("Colour", m_player.color);

        clamp_player_to_window();
    }

    void SandboxScene::draw_debug_ui() {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Player size: %d", m_player.size);
    }

    void SandboxScene::clamp_player_to_window() {
        m_player.x = std::clamp(m_player.x, 0.0f, static_cast<float>(m_window_width - m_player.size));
        m_player.y = std::clamp(m_player.y, 0.0f, static_cast<float>(m_window_height - m_player.size));
    }
}
