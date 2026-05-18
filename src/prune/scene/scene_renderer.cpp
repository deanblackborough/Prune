#include <algorithm>
#include <cmath>

#include <SDL_image.h>

#include "prune/scene/scene_renderer.hpp"
#include "prune/resources/sprites.hpp"

namespace prune {

    SceneRenderer::~SceneRenderer()
    {
        clear_cached_textures();
    }

    void SceneRenderer::clear_cached_textures() noexcept
    {
        for (auto& [key, texture] : m_sprite_textures) {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }

        m_sprite_textures.clear();
    }

    bool SceneRenderer::is_rect_visible(const SceneViewport& viewport, const SDL_Rect& rect) noexcept
    {
        return rect.x + rect.w >= 0 &&
            rect.y + rect.h >= 0 &&
            rect.x < viewport.width &&
            rect.y < viewport.height;
    }

    void SceneRenderer::draw_grid(SDL_Renderer* renderer, const SceneViewport& viewport, const SceneCamera& scene_camera, const GridOptions& grid_options) const
    {
        if (!grid_options.show_grid || grid_options.grid_size <= 1) {
            return;
        }

        const int grid_size = std::max(1, grid_options.grid_size);
        const int major_every = 4; // 16px * 4 = 64px

        const Camera& camera = scene_camera.active();

        const float zoom = std::max(camera.zoom, 0.01f);

        const float left_world = camera.x;
        const float right_world = camera.x + (static_cast<float>(viewport.width) / zoom);
        const float top_world = camera.y;
        const float bottom_world = camera.y + (static_cast<float>(viewport.height) / zoom);

        const float first_vertical_world =
            std::floor(left_world / static_cast<float>(grid_size)) * static_cast<float>(grid_size);

        const float first_horizontal_world =
            std::floor(top_world / static_cast<float>(grid_size)) * static_cast<float>(grid_size);

        const int vertical_line_count =
            static_cast<int>(std::ceil((right_world - first_vertical_world) / static_cast<float>(grid_size))) + 1;

        for (int i = 0; i < vertical_line_count; ++i) {
            const float world_x = first_vertical_world + static_cast<float>(i * grid_size);

            if (world_x > right_world) {
                break;
            }

            const int grid_index = static_cast<int>(std::round(world_x / static_cast<float>(grid_size)));
            const bool is_origin = grid_index == 0;
            const bool is_major = grid_index % major_every == 0;

            if (is_origin) {
                SDL_SetRenderDrawColor(renderer, 130, 90, 160, 110);
            }
            else if (is_major) {
                SDL_SetRenderDrawColor(renderer, 74, 52, 96, 70);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 48, 34, 64, 55);
            }

            const int screen_x = static_cast<int>(std::round((world_x - camera.x) * zoom));
            SDL_RenderDrawLine(renderer, screen_x, 0, screen_x, viewport.height);
        }

        const int horizontal_line_count =
            static_cast<int>(std::ceil((bottom_world - first_horizontal_world) / static_cast<float>(grid_size))) + 1;

        for (int i = 0; i < horizontal_line_count; ++i) {
            const float world_y = first_horizontal_world + static_cast<float>(i * grid_size);

            if (world_y > bottom_world) {
                break;
            }

            const int grid_index = static_cast<int>(std::round(world_y / static_cast<float>(grid_size)));
            const bool is_origin = grid_index == 0;
            const bool is_major = grid_index % major_every == 0;

            if (is_origin) {
                SDL_SetRenderDrawColor(renderer, 130, 90, 160, 110);
            }
            else if (is_major) {
                SDL_SetRenderDrawColor(renderer, 74, 52, 96, 90);
            }
            else {
                SDL_SetRenderDrawColor(renderer, 48, 34, 64, 55);
            }

            const int screen_y = static_cast<int>(std::round((world_y - camera.y) * zoom));
            SDL_RenderDrawLine(renderer, 0, screen_y, viewport.width, screen_y);
        }
    }

    void SceneRenderer::draw_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object, SDL_Rect& selected_outline, bool& has_selected_outline)
    {
        switch (object.render.type) {
        case RenderType::Rectangle:
            draw_rectangle_object(renderer, state, camera, object, selected_outline, has_selected_outline);
            break;

        case RenderType::Sprite:
            draw_sprite_object(renderer, state, camera, object, selected_outline, has_selected_outline);
            break;

        default:
            break;
        }
    }

    void SceneRenderer::draw_rectangle_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object, SDL_Rect& selected_outline, bool& has_selected_outline) const
    {
        SDL_Rect rect = camera.world_to_screen_rect(object);

        if (!is_rect_visible(state.viewport, rect)) {
            return;
        }

        SDL_SetRenderDrawColor(renderer,
            static_cast<Uint8>(object.render.rectangle.color[0] * 255.0f),
            static_cast<Uint8>(object.render.rectangle.color[1] * 255.0f),
            static_cast<Uint8>(object.render.rectangle.color[2] * 255.0f),
            255
        );

        SDL_RenderFillRect(renderer, &rect);

        capture_selected_outline(state, object, rect, selected_outline, has_selected_outline);
    }

    void SceneRenderer::draw_sprite_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object, SDL_Rect& selected_outline, bool& has_selected_outline)
    {
        SDL_Rect rect = camera.world_to_screen_rect(object);

        if (!is_rect_visible(state.viewport, rect)) {
            return;
        }

        SDL_Texture* texture = sprite_texture(renderer, object.render.sprite.sprite_key);

        if (texture) {
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
        }
        else {
            draw_sprite_fallback(renderer, rect);
        }

        capture_selected_outline(state, object, rect, selected_outline, has_selected_outline);
    }

    void SceneRenderer::render(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GridOptions& grid_options)
    {
        if (!state.viewport.has_area()) {
            return;
        }

        draw_grid(renderer, state.viewport, camera, grid_options);

        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : state.objects.objects()) {
            if (!object.lifecycle.active || !object.render.visible) {
                continue;
            }

            draw_object(renderer, state, camera, object, selected_outline, has_selected_outline);
        }

        if (state.scene_options.highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 174, 99, 242, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SceneRenderer::draw_sprite_fallback(SDL_Renderer* renderer, const SDL_Rect& rect) const
    {
        SDL_SetRenderDrawColor(renderer, 38, 28, 48, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 174, 99, 242, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x, rect.y + rect.h);
    }

    void SceneRenderer::capture_selected_outline(const SceneState& state, const GameObject& object, const SDL_Rect& rect, SDL_Rect& selected_outline, bool& has_selected_outline) const noexcept
    {
        if (object.identity.id == state.objects.selected_id()) {
            selected_outline = rect;
            has_selected_outline = true;
        }
    }

    SDL_Texture* SceneRenderer::sprite_texture(SDL_Renderer* renderer, const std::string& sprite_key)
    {
        if (sprite_key.empty()) {
            return nullptr;
        }

        if (const auto existing = m_sprite_textures.find(sprite_key); existing != m_sprite_textures.end()) {
            return existing->second;
        }

        const Sprites* resource = find_sprite_resource(sprite_key);
        if (!resource) {
            m_sprite_textures.emplace(sprite_key, nullptr);
            return nullptr;
        }

        SDL_Texture* texture = IMG_LoadTexture(renderer, resource->path.data());

        m_sprite_textures.emplace(sprite_key, texture);

        return texture;
    }

}
