#pragma once

#include <array>
#include <string>

#include <SDL2/SDL.h>

#include "prune/scene/scene_state.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_interaction.hpp"

namespace prune {

    class SandboxScene {
    public:
        SandboxScene(int window_width, int window_height);
        SandboxScene(const SandboxScene&) = delete;
        SandboxScene& operator=(const SandboxScene&) = delete;

        void on_enter();
        void on_exit();
        void update(float dt, const Input& input);
        void render(SDL_Renderer* renderer);

        void new_scene();
        [[nodiscard]] SceneState& get_state() noexcept;
        [[nodiscard]] const SceneState& get_state() const noexcept;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error);

        void set_viewport(const SceneViewport& viewport) noexcept;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept { return m_state.viewport.height; }

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
        const Camera& get_active_camera() const noexcept;

        SimpleShooterOptions& get_simple_shooter_options() noexcept;
        const SimpleShooterOptions& get_simple_shooter_options() const noexcept;

        void reset_simple_shooter();

        [[nodiscard]] int bullet_count() const noexcept;

        [[nodiscard]] GameObject* enemy_object() noexcept;
        [[nodiscard]] const GameObject* enemy_object() const noexcept;

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
        [[nodiscard]] static GameObject create_enemy();

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

        void update_player_facing(GameObject& player) noexcept;
        void handle_player_shooting(const Input& input);
        void respawn_enemy(GameObject& enemy);
        void create_bullet_from_player(const GameObject& player);
        void update_enemy(float dt);
        void update_bullets(float dt);
        void handle_bullet_enemy_collisions();
        void cleanup_runtime_objects();

        SceneState m_state;

        SceneRenderer m_renderer;

        SceneInteraction m_interaction;
    };
}