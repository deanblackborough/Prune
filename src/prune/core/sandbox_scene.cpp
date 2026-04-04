#include "sandbox_scene.hpp"

#include "imgui.h"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <string>

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::on_enter()
    {
        m_objects.clear();

        GameObject player;
        player.name = "Player";
        player.transform.x = 100.0f;
        player.transform.y = 100.0f;
        player.rectangle.width = 50;
        player.rectangle.height = 50;
        player.rectangle.color[0] = 0.3f;
        player.rectangle.color[1] = 0.8f;
        player.rectangle.color[2] = 0.5f;

        m_player_index = m_objects.create_object(player);

        GameObject block;
        block.name = "Static Block";
        block.transform.x = 420.0f;
        block.transform.y = 220.0f;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;

        m_objects.create_object(block);
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        if (GameObject* player = player_object()) {
            m_player_controller.update(
                *player,
                dt,
                input,
                m_window_width,
                m_window_height
            );
        }
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        for (const auto& object : m_objects.objects()) {
            SDL_Rect rect{
                static_cast<int>(object.transform.x),
                static_cast<int>(object.transform.y),
                object.rectangle.width,
                object.rectangle.height
            };

            SDL_SetRenderDrawColor(
                renderer,
                static_cast<Uint8>(object.rectangle.color[0] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[1] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[2] * 255.0f),
                255
            );

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    void SandboxScene::draw_inspector_ui()
    {
        ImGui::TextUnformatted("Objects");
        ImGui::Separator();

        for (std::size_t index = 0; index < m_objects.count(); ++index) {
            const GameObject* object = m_objects.get(index);
            if (!object) {
                continue;
            }

            const bool is_selected = index == m_objects.selected_index();
            if (ImGui::Selectable(object->name.c_str(), is_selected)) {
                m_objects.select(index);
            }
        }

        ImGui::Spacing();

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        ImGui::Separator();
        ImGui::Text("Selected: %s", selected->name.c_str());

        ImGui::Spacing();
        ImGui::TextUnformatted("Position");
        ImGui::SliderFloat("X", &selected->transform.x, 0.0f, static_cast<float>(m_window_width));
        ImGui::SliderFloat("Y", &selected->transform.y, 0.0f, static_cast<float>(m_window_height));

        ImGui::Separator();

        ImGui::TextUnformatted("Rectangle");
        ImGui::SliderInt("Width", &selected->rectangle.width, 10, 200);
        ImGui::SliderInt("Height", &selected->rectangle.height, 10, 200);
        ImGui::ColorEdit3("Colour", selected->rectangle.color);

        if (m_objects.selected_index() == m_player_index) {
            float speed = m_player_controller.speed();
            if (ImGui::SliderFloat("Move Speed", &speed, 50.0f, 600.0f, "%.1f")) {
                m_player_controller.set_speed(speed);
            }
        }

        selected->clamp_to_area(m_window_width, m_window_height);
    }

    void SandboxScene::draw_debug_ui()
    {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Object count: %d", static_cast<int>(m_objects.count()));

        if (const GameObject* player = player_object()) {
            ImGui::Separator();
            ImGui::TextUnformatted("Player");
            ImGui::Text("Position: %.1f, %.1f", player->transform.x, player->transform.y);
            ImGui::Text("Speed: %.1f", m_player_controller.speed());
        }
    }

    GameObject* SandboxScene::player_object() noexcept
    {
        return m_objects.get(m_player_index);
    }

    const GameObject* SandboxScene::player_object() const noexcept
    {
        return m_objects.get(m_player_index);
    }

} // namespace prune