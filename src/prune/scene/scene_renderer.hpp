#pragma once

#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class Scene;

    class SceneRenderer {
    public:
        SceneRenderer() = default;
        ~SceneRenderer();

        SceneRenderer(const SceneRenderer&) = delete;
        SceneRenderer& operator=(const SceneRenderer&) = delete;

        void render(SDL_Renderer* renderer, const Scene& scene, const SceneState& state, const SceneCamera& camera, const GridOptions& grid_options);
        void clear_cached_textures() noexcept;

    private:
        [[nodiscard]] static bool is_rect_visible(const SceneViewport& viewport, const SDL_Rect& rect) noexcept;
        [[nodiscard]] static SDL_Rect expanded_rect(const SDL_Rect& rect, int amount) noexcept;

        void draw_grid(SDL_Renderer* renderer, const SceneViewport& viewport, const SceneCamera& camera, const GridOptions& grid_options) const;
        void draw_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object);
        void draw_rectangle_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object) const;
        void draw_sprite_object(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object);
        void draw_sprite_fallback(SDL_Renderer* renderer, const SDL_Rect& rect) const;
        void draw_debug_overlays(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera, const GameObject& object) const;
        void draw_selected_gizmo(SDL_Renderer* renderer, const SDL_Rect& selected_outline, bool movable) const;
        void draw_selected_outline(SDL_Renderer* renderer, const Scene& scene, const SceneState& state, const SceneCamera& camera, const GameObject& object) const;
        void draw_multi_selection_bounds(SDL_Renderer* renderer, const SceneState& state, const SceneCamera& camera) const;
        [[nodiscard]] bool selected_screen_bounds(const SceneState& state, const SceneCamera& camera, SDL_Rect& bounds) const;

        [[nodiscard]] SDL_Texture* sprite_texture(SDL_Renderer* renderer, const std::string& sprite_key);

        std::unordered_map<std::string, SDL_Texture*> m_sprite_textures;
    };
}
