#include "prune/scene/sandbox_scene.hpp"

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

    void SandboxScene::on_exit() {
        m_objects.clear();
    }

    GameObject SandboxScene::create_player() {
        GameObject player;
        player.name = "Player";
        player.transform.x = 128.0f;
        player.transform.y = 128.0f;
        player.rectangle.width = 32;
        player.rectangle.height = 32;
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
        block.transform.x = 128.0f;
        block.transform.y = 256.0f;
        block.rectangle.width = 32;
        block.rectangle.height = 32;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        return block;
    }

    GameObjectManager& SandboxScene::get_object_manager() {
        return m_objects;
    }

    GameObjectId SandboxScene::get_player_id() const {
        return m_player_id;
    }

    PlayerController & SandboxScene::get_player_controller() {
        return m_player_controller;
    }

    GridOptions& SandboxScene::get_grid_options() {
        return m_grid_options;
    }

    SceneOptions& SandboxScene::get_scene_options() {
        return m_scene_options;
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        update_editor(dt, input);
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

        player->velocity = m_player_controller.movement_velocity(input);
        move_object(*player, player->velocity.x * dt, player->velocity.y * dt, true);
    }

    void SandboxScene::move_object(GameObject& object, float delta_x, float delta_y, bool resolve_collisions)
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_collisions && object.is_player) {
            resolve_player_collisions(object);
        }
    }

    Transform SandboxScene::screen_to_world(int screen_x, int screen_y) const noexcept
    {
        return {
            static_cast<float>(screen_x) + camera_x,
            static_cast<float>(screen_y) + camera_y
        };
    }

    SDL_Rect SandboxScene::world_to_screen_rect(const GameObject& object) const noexcept
    {
        return SDL_Rect{
            static_cast<int>(std::round(object.transform.x - camera_x)),
            static_cast<int>(std::round(object.transform.y - camera_y)),
            object.rectangle.width,
            object.rectangle.height
        };
    }

    bool SandboxScene::is_rect_visible(const SDL_Rect& rect) const noexcept
    {
        return rect.x + rect.w >= 0 &&
               rect.y + rect.h >= 0 &&
               rect.x < m_window_width &&
               rect.y < m_window_height;
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        draw_grid(renderer);

        const GameObjectId selected_id = m_objects.selected_id();
        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : m_objects.objects()) {
            if (!object.active || !object.visible) {
                continue;
            }

            const SDL_Rect rect = world_to_screen_rect(object);

            if (!is_rect_visible(rect)) {
                continue;
            }

            SDL_SetRenderDrawColor(
                renderer,
                static_cast<Uint8>(object.rectangle.color[0] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[1] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[2] * 255.0f),
                255
            );

            SDL_RenderFillRect(renderer, &rect);

            if (m_scene_options.highlight_selected && object.id == selected_id) {
                selected_outline = SDL_Rect{
                    rect.x - 2,
                    rect.y - 2,
                    rect.w + 4,
                    rect.h + 4
                };

                has_selected_outline = true;
            }
        }

        if (m_scene_options.highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SandboxScene::draw_grid(SDL_Renderer* renderer) const
    {
        if (!m_grid_options.show_grid || m_grid_options.grid_size <= 1) {
            return;
        }

        const int scene_grid_size = std::max(1, m_grid_options.grid_size);

        const float left_world = camera_x;
        const float right_world = camera_x + static_cast<float>(m_window_width);
        const float top_world = camera_y;
        const float bottom_world = camera_y + static_cast<float>(m_window_height);

        const float first_vertical_world =
            std::floor(left_world / static_cast<float>(scene_grid_size)) * static_cast<float>(scene_grid_size);

        const float first_horizontal_world =
            std::floor(top_world / static_cast<float>(scene_grid_size)) * static_cast<float>(scene_grid_size);

        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);

        const int vertical_line_count = static_cast<int>(std::ceil((right_world - first_vertical_world) / static_cast<float>(scene_grid_size))) + 1;
        for (int i = 0; i < vertical_line_count; ++i) {
            const float world_x = first_vertical_world + static_cast<float>(i * scene_grid_size);
            if (world_x > right_world) {
                break;
            }
            const int screen_x = static_cast<int>(std::round(world_x - camera_x));
            SDL_RenderDrawLine(renderer, screen_x, 0, screen_x, m_window_height);
        }

        const int horizontal_line_count = static_cast<int>(std::ceil((bottom_world - first_horizontal_world) / static_cast<float>(scene_grid_size))) + 1;
        for (int i = 0; i < horizontal_line_count; ++i) {
            const float world_y = first_horizontal_world + static_cast<float>(i * scene_grid_size);
            if (world_y > bottom_world) {
                break;
            }
            const int screen_y = static_cast<int>(std::round(world_y - camera_y));
            SDL_RenderDrawLine(renderer, 0, screen_y, m_window_width, screen_y);
        }
    }

    float SandboxScene::snap_value_to_grid(float value) const noexcept
    {
        const int scene_grid_size = std::max(1, m_grid_options.grid_size);
        return std::round(value / static_cast<float>(scene_grid_size)) * static_cast<float>(scene_grid_size);
    }

    void SandboxScene::snap_object_to_grid(GameObject& object) const noexcept
    {
        object.transform.x = snap_value_to_grid(object.transform.x);
        object.transform.y = snap_value_to_grid(object.transform.y);
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