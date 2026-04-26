#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL_image.h>
#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/resources/sprites.hpp"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
    {
        m_viewport.width = window_width;
        m_viewport.height = window_height;
    }

    SandboxScene::~SandboxScene()
    {
        destroy_sprite_textures();
    }

    void SandboxScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_viewport = viewport;
    }

    bool SandboxScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_viewport.focused && m_viewport.has_area();
    }

    bool SandboxScene::scene_mouse_input_enabled() const noexcept
    {
        return m_viewport.hovered && m_viewport.has_area();
    }

    bool SandboxScene::scene_input_enabled() const noexcept
    {
        return scene_keyboard_input_enabled();
    }

    void SandboxScene::on_enter()
    {
        new_scene();
    }

    void SandboxScene::on_exit() {
        m_objects.clear();
        m_player_id = kInvalidGameObjectId;
    }

    GameObject SandboxScene::create_player() {
        GameObject player;
        player.name = "Player";
		player.kind = GameObjectKind::Player;
		player.size.width = 16;
		player.size.height = 16;
		player.render.type = RenderType::Sprite;
		player.render.sprite.sprite_key = "tank-green";
		player.render.rectangle.color[0] = 0.3f;
		player.render.rectangle.color[1] = 0.8f;
		player.render.rectangle.color[2] = 0.5f;
        player.transform.x = 128.0f;
        player.transform.y = 128.0f;
        player.active = true;
        player.render.visible = true;
        player.collision.solid = false;

        return player;
    }

    GameObject SandboxScene::create_initial_block() {
        GameObject block;
        block.name = "Static Block";
		block.kind = GameObjectKind::Block;
        block.transform.x = 128.0f;
        block.transform.y = 256.0f;
        block.size.width = 16;
        block.size.height = 16;
        block.render.type = RenderType::Rectangle;
        block.render.rectangle.color[0] = 0.8f;
        block.render.rectangle.color[1] = 0.5f;
        block.render.rectangle.color[2] = 0.2f;
        block.active = true;
        block.collision.solid = true;

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


    CameraState& SandboxScene::get_camera_state() noexcept {
        return m_cameras;
    }

    const CameraState& SandboxScene::get_camera_state() const noexcept {
        return m_cameras;
    }

    Camera& SandboxScene::get_editor_camera() noexcept {
        return m_cameras.editor;
    }

    const Camera& SandboxScene::get_editor_camera() const noexcept {
        return m_cameras.editor;
    }

    Camera& SandboxScene::get_game_camera() noexcept {
        return m_cameras.game;
    }

    const Camera& SandboxScene::get_game_camera() const noexcept {
        return m_cameras.game;
    }

    Camera& SandboxScene::get_active_camera() noexcept {
        return (m_cameras.mode == CameraMode::Editor) ? m_cameras.editor : m_cameras.game;
    }

    const Camera& SandboxScene::get_active_camera() const noexcept {
        return (m_cameras.mode == CameraMode::Editor) ? m_cameras.editor : m_cameras.game;
    }

    void SandboxScene::activate_editor_camera() noexcept
    {
        m_cameras.mode = CameraMode::Editor;
    }

    void SandboxScene::activate_game_camera() noexcept
    {
        m_cameras.mode = CameraMode::Game;
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        update_game(dt, input);
        update_editor(dt, input);
		update_cameras();
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

        if (!scene_keyboard_input_enabled()) {
            player->velocity = {};
            return;
        }

        player->velocity = m_player_controller.movement_velocity(input);

        const bool is_moving =
            player->velocity.x != 0.0f ||
            player->velocity.y != 0.0f;

        if (is_moving) {
            activate_game_camera();
        }

        move_object(*player, player->velocity.x * dt, player->velocity.y * dt, true);
    }

    void SandboxScene::update_cameras()
    {
        update_game_camera();
    }

    void SandboxScene::update_game_camera() noexcept
    {
        const GameObject* player = player_object();
        if (!player) {
            return;
        }

        if (!m_cameras.game_options.follow_player) {
            return;
        }

        const float player_center_x =
            player->transform.x + (static_cast<float>(player->size.width) * 0.5f);

        const float player_center_y =
            player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

        if (!m_viewport.has_area()) {
            return;
        }

        const float zoom = std::max(m_cameras.game.zoom, 0.01f);

        m_cameras.game.x = player_center_x - ((static_cast<float>(m_viewport.width) / zoom) * 0.5f);
        m_cameras.game.y = player_center_y - ((static_cast<float>(m_viewport.height) / zoom) * 0.5f);
    }

    void SandboxScene::move_object(GameObject& object, float delta_x, float delta_y, bool resolve_collisions)
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_collisions && object.kind == GameObjectKind::Player) {
            resolve_player_collisions(object);
        }
    }

    Transform SandboxScene::screen_to_world(int screen_x, int screen_y) const noexcept
    {
        const Camera& camera = get_active_camera();

        const int local_x = screen_x - m_viewport.screen_x;
        const int local_y = screen_y - m_viewport.screen_y;

        return {
            camera.x + static_cast<float>(local_x) / camera.zoom,
            camera.y + static_cast<float>(local_y) / camera.zoom
        };
    }

    SDL_Rect SandboxScene::world_to_screen_rect(const GameObject& object) const noexcept
    {
        const Camera& camera = get_active_camera();

        const float zoom = std::max(camera.zoom, 0.01f);

        return SDL_Rect{
            static_cast<int>(std::round((object.transform.x - camera.x) * zoom)),
            static_cast<int>(std::round((object.transform.y - camera.y) * zoom)),
            static_cast<int>(std::round(static_cast<float>(object.size.width) * zoom)),
            static_cast<int>(std::round(static_cast<float>(object.size.height) * zoom))
        };
    }

    bool SandboxScene::is_rect_visible(const SDL_Rect& rect) const noexcept
    {
        return rect.x + rect.w >= 0 &&
            rect.y + rect.h >= 0 &&
            rect.x < m_viewport.width &&
            rect.y < m_viewport.height;
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        if (!m_viewport.has_area()) {
            return;
        }

        draw_grid(renderer);

        const GameObjectId selected_id = m_objects.selected_id();
        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : m_objects.objects()) {
            if (!object.active || !object.render.visible) {
                continue;
            }

            switch (object.render.type) {
                case RenderType::Rectangle: {
                    const SDL_Rect rect = world_to_screen_rect(object);

                    if (!is_rect_visible(rect)) {
                        continue;
                    }

                    SDL_SetRenderDrawColor(
                        renderer,
                        static_cast<Uint8>(object.render.rectangle.color[0] * 255.0f),
                        static_cast<Uint8>(object.render.rectangle.color[1] * 255.0f),
                        static_cast<Uint8>(object.render.rectangle.color[2] * 255.0f),
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

                    break;
                }
                case RenderType::Sprite: {
                    const SDL_Rect rect = world_to_screen_rect(object);

                    if (!is_rect_visible(rect)) {
                        continue;
                    }

                    SDL_Texture* texture = sprite_texture(renderer, object.render.sprite.sprite_key);

                    if (texture) {
                        SDL_RenderCopy(renderer, texture, nullptr, &rect);
                    }
                    else {
                        draw_sprite_fallback(renderer, rect);
                    }

                    if (m_scene_options.highlight_selected && object.id == selected_id) {
                        selected_outline = SDL_Rect{
                            rect.x - 2,
                            rect.y - 2,
                            rect.w + 4,
                            rect.h + 4
                        };

                        has_selected_outline = true;
                    }

                    break;
                }
			}
        }

        if (m_scene_options.highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 174, 99, 242, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SandboxScene::draw_grid(SDL_Renderer* renderer) const
    {
        if (!m_grid_options.show_grid || m_grid_options.grid_size <= 1) {
            return;
        }

        const int grid_size = std::max(1, m_grid_options.grid_size);
        const int major_every = 4; // 16px * 4 = 64px

        const Camera& camera = get_active_camera();

        const float zoom = std::max(camera.zoom, 0.01f);

        const float left_world = camera.x;
        const float right_world = camera.x + (static_cast<float>(m_viewport.width) / zoom);
        const float top_world = camera.y;
        const float bottom_world = camera.y + (static_cast<float>(m_viewport.height) / zoom);

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
            SDL_RenderDrawLine(renderer, screen_x, 0, screen_x, m_viewport.height);
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
            SDL_RenderDrawLine(renderer, 0, screen_y, m_viewport.width, screen_y);
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
            if (object.id == player.id || !object.active || !object.collision.solid) {
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

    void SandboxScene::reset_runtime_state()
    {
        m_objects.clear();
        m_player_id = kInvalidGameObjectId;

        m_cameras = {};
        m_cameras.editor.speed = 256.0f;
        m_cameras.editor.zoom = 1.0f;

        m_cameras.game.speed = 256.0F;
        m_cameras.game.zoom = 3.0f;

        m_cameras.mode = CameraMode::Editor;
        m_cameras.game_options.follow_player = true;

        m_grid_options = {};
        m_scene_options = {};
        m_player_controller = {};
    }

    void SandboxScene::restore_defaults()
    {
        reset_runtime_state();

        m_player_id = m_objects.create_object(create_player());
        m_objects.create_object(create_initial_block());
        m_objects.select(m_player_id);

		update_game_camera();
    }

    void SandboxScene::new_scene()
    {
        restore_defaults();

        update_game_camera();
    }

    void SandboxScene::destroy_sprite_textures() noexcept
    {
        for (auto& [key, texture] : m_sprite_textures) {
            if (texture) {
                SDL_DestroyTexture(texture);
            }
        }

        m_sprite_textures.clear();
    }

    SDL_Texture* SandboxScene::sprite_texture(SDL_Renderer* renderer, const std::string& sprite_key)
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

    void SandboxScene::draw_sprite_fallback(SDL_Renderer* renderer, const SDL_Rect& rect) const
    {
        SDL_SetRenderDrawColor(renderer, 38, 28, 48, 255);
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 174, 99, 242, 255);
        SDL_RenderDrawRect(renderer, &rect);

        SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y + rect.h);
        SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x, rect.y + rect.h);
    }
}