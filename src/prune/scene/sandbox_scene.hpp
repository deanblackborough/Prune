#pragma once

#include <array>
#include <string>
#include <unordered_map>

#include <SDL2/SDL.h>

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/player_controller.hpp"

namespace prune {

    struct SceneViewport {
        int screen_x = 0;
        int screen_y = 0;
        int width = 0;
        int height = 0;
        bool hovered = false;
        bool focused = false;

        [[nodiscard]] bool has_area() const noexcept {
            return width > 0 && height > 0;
        }

        [[nodiscard]] bool contains(int x, int y) const noexcept {
            return x >= screen_x &&
                y >= screen_y &&
                x < (screen_x + width) &&
                y < (screen_y + height);
        }
    };

    struct SceneOptions {
        bool highlight_selected = true;
    };

    struct Camera {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 256.0f;
        float zoom = 1.0f;
    };

    enum class CameraMode {
        Editor = 0,
		Game = 1
    };

    struct GameCameraOptions {
		bool follow_player = true;
    };

    struct CameraState {
        Camera editor;
        Camera game;
		CameraMode mode = CameraMode::Editor;
		GameCameraOptions game_options;
    };

    struct GridOptions {
        bool show_grid = true;
        bool snap_to_grid = true;
        int grid_size = k_default_object_size;
        int nudge_step = 8;
        int shift_nudge_steps = 4;

        int min_grid_size = k_min_object_size;
        int max_grid_size = k_max_object_size;
        int min_nudge_step = 4;
        int max_nudge_step = 64;
    };

    class SandboxScene {
    public:
        SandboxScene(int window_width, int window_height);
        ~SandboxScene();
        SandboxScene(const SandboxScene&) = delete;
        SandboxScene& operator=(const SandboxScene&) = delete;

        void on_enter();
        void on_exit();
        void update(float dt, const Input& input);
        void render(SDL_Renderer* renderer);

        void new_scene();

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error);

        void set_viewport(const SceneViewport& viewport) noexcept;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept { return m_viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept { return m_viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept { return m_viewport.height; }

        GameObjectManager& get_object_manager();
        [[nodiscard]] GameObjectId get_player_id() const;
        PlayerController& get_player_controller();
        GridOptions& get_grid_options();
        SceneOptions& get_scene_options();

        CameraState& get_camera_state() noexcept;
        const CameraState& get_camera_state() const noexcept;

        Camera& get_editor_camera() noexcept;
        const Camera& get_editor_camera() const noexcept;

        Camera& get_game_camera() noexcept;
        const Camera& get_game_camera() const noexcept;

        Camera& get_active_camera() noexcept;
        const Camera& get_active_camera() const noexcept;;

    private:
        void reset_runtime_state();
        void restore_defaults();

        [[nodiscard]] bool scene_input_enabled() const noexcept;
        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        [[nodiscard]] static GameObject create_player();
        [[nodiscard]] static GameObject create_initial_block();

        // Game logic
        void update_game(float dt, const Input& input);
        void update_player(float dt, const Input& input);
        void update_cameras();
        void update_game_camera() noexcept;

        void activate_editor_camera() noexcept;
        void activate_game_camera() noexcept;

        void move_object(GameObject& object, float delta_x, float delta_y, bool resolve_collisions);
        void resolve_player_collisions(GameObject& player);
        [[nodiscard]] bool is_overlapping(const GameObject& a, const GameObject& b) const noexcept;

        // Editor logic
        void update_editor(float dt, const Input& input);
        void update_editor_camera(float dt, const Input& input);
        void handle_scene_click(const Input& input);
        void handle_keyboard_nudge(const Input& input);
        [[nodiscard]] GameObject* pick_object_at_screen(int screen_x, int screen_y) noexcept;

        // Camera logic
        [[nodiscard]] Transform screen_to_world(int screen_x, int screen_y) const noexcept;
        [[nodiscard]] SDL_Rect world_to_screen_rect(const GameObject& object) const noexcept;
        [[nodiscard]] bool is_rect_visible(const SDL_Rect& rect) const noexcept;

        // Editor grid
        void draw_grid(SDL_Renderer* renderer) const;
        [[nodiscard]] float snap_value_to_grid(float value) const noexcept;
        void snap_object_to_grid(GameObject& object) const noexcept;

		// Sprites
        void destroy_sprite_textures() noexcept;
        [[nodiscard]] SDL_Texture* sprite_texture(SDL_Renderer* renderer, const std::string& sprite_key);
        void draw_sprite_fallback(SDL_Renderer* renderer, const SDL_Rect& rect) const;

        GameObjectManager m_objects;
        PlayerController m_player_controller;

        GameObjectId m_player_id = k_invalid_game_object_id;

        SceneViewport m_viewport{};

        GridOptions m_grid_options;
        SceneOptions m_scene_options;

        CameraState m_cameras;

        std::unordered_map<std::string, SDL_Texture*> m_sprite_textures;
    };
}