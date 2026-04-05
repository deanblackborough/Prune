#include "sandbox_scene.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

#include "imgui.h"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::on_enter()
    {
        m_objects.clear();

        m_player_id = m_objects.create_object(create_player());
        m_objects.create_object(create_initial_block());

        m_objects.select(m_player_id);
    }

    GameObject SandboxScene::create_player() {
        GameObject player;
        player.name = "Player";
        player.transform.x = 100.0f;
        player.transform.y = 100.0f;
        player.rectangle.width = 50;
        player.rectangle.height = 50;
        player.rectangle.color[0] = 0.3f;
        player.rectangle.color[1] = 0.8f;
        player.rectangle.color[2] = 0.5f;
        player.active = true;
        player.visible = true;
        player.solid = false;
        player.is_player = true;

        return player;
    }

    GameObject SandboxScene::create_initial_block() {
        GameObject block;
        block.name = "Static Block";
        block.transform.x = 420.0f;
        block.transform.y = 220.0f;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        return block;
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        update_editor(input);
        update_game(dt, input);
    }

    void SandboxScene::update_game(float dt, const Input& input)
    {
        update_player(dt, input);
    }

    void SandboxScene::update_player(float dt, const Input& input)
    {
        GameObject* player = player_object();
        if (!player) {
            return;
        }

        m_player_controller.update(*player, dt, input);
        resolve_player_collisions(*player);
        player->clamp_to_area(m_window_width, m_window_height);
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        const GameObjectId selected_id = m_objects.selected_id();
        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : m_objects.objects()) {
            if (!object.active || !object.visible) {
                continue;
            }

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

            if (m_editor_state.highlight_selected && object.id == selected_id) {
                selected_outline = SDL_Rect{
                    rect.x - 2,
                    rect.y - 2,
                    rect.w + 4,
                    rect.h + 4
                };

                has_selected_outline = true;
            }
        }

        if (m_editor_state.highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SandboxScene::draw_inspector_ui()
    {
        if (ImGui::Button("Add Block")) {
            const Transform spawn = next_block_spawn_position();
            create_block(spawn.x, spawn.y);
        }

        ImGui::Checkbox("Highlight selected", &m_editor_state.highlight_selected);

        ImGui::Separator();

        draw_object_list_ui();

        ImGui::Spacing();

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        draw_selected_object_ui();
    }

    void SandboxScene::draw_debug_ui()
    {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Object count: %d", static_cast<int>(m_objects.count()));
        ImGui::Text("Selected id: %u", m_objects.selected_id());
        ImGui::Text("Player id: %u", m_player_id);

        if (const GameObject* player = player_object()) {
            ImGui::Separator();
            ImGui::TextUnformatted("Player");
            ImGui::Text("Position: %.1f, %.1f", player->transform.x, player->transform.y);
            ImGui::Text("Velocity: %.1f, %.1f", player->velocity.x, player->velocity.y);
            ImGui::Text("Speed: %.1f", m_player_controller.speed());
        }
    }

    GameObject* SandboxScene::player_object() noexcept
    {
        return m_objects.get_by_id(m_player_id);
    }

    const GameObject* SandboxScene::player_object() const noexcept
    {
        return m_objects.get_by_id(m_player_id);
    }

    bool SandboxScene::is_overlapping(const GameObject& a, const GameObject& b) const noexcept
    {
        const RectF a_bounds = a.bounds();
        const RectF b_bounds = b.bounds();

        return a_bounds.x < (b_bounds.x + b_bounds.width) &&
               (a_bounds.x + a_bounds.width) > b_bounds.x &&
               a_bounds.y < (b_bounds.y + b_bounds.height) &&
               (a_bounds.y + a_bounds.height) > b_bounds.y;
    }

    Transform SandboxScene::next_block_spawn_position() const noexcept
    {
        constexpr float base_x = 160.0f;
        constexpr float base_y = 160.0f;
        constexpr float offset_step = 50.0f;
        constexpr int block_size = 50;

        const float offset = static_cast<float>(m_objects.count()) * offset_step;

        Transform spawn{
            base_x + offset,
            base_y + offset
        };

        const float max_x = std::max(0.0f, static_cast<float>(m_window_width - block_size));
        const float max_y = std::max(0.0f, static_cast<float>(m_window_height - block_size));

        spawn.x = std::clamp(spawn.x, 0.0f, max_x);
        spawn.y = std::clamp(spawn.y, 0.0f, max_y);

        return spawn;
    }

    void SandboxScene::resolve_player_collisions(GameObject& player)
    {
        for (const auto& object : m_objects.objects()) {
            if (object.id == player.id || !object.active || !object.solid) {
                continue;
            }

            if (!is_overlapping(player, object)) {
                continue;
            }

            const RectF player_bounds = player.bounds();
            const RectF block_bounds = object.bounds();

            const float overlap_left = (player_bounds.x + player_bounds.width) - block_bounds.x;
            const float overlap_right = (block_bounds.x + block_bounds.width) - player_bounds.x;
            const float overlap_top = (player_bounds.y + player_bounds.height) - block_bounds.y;
            const float overlap_bottom = (block_bounds.y + block_bounds.height) - player_bounds.y;

            const float resolve_x = (overlap_left < overlap_right) ? -overlap_left : overlap_right;
            const float resolve_y = (overlap_top < overlap_bottom) ? -overlap_top : overlap_bottom;

            if (std::abs(resolve_x) < std::abs(resolve_y)) {
                player.transform.x += resolve_x;
                player.velocity.x = 0.0f;
            } else {
                player.transform.y += resolve_y;
                player.velocity.y = 0.0f;
            }
        }
    }
} // namespace prune