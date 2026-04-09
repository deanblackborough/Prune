#pragma once

#include <array>
#include <random>
#include <string>

#include "game_object.hpp"
#include "game_object_manager.hpp"
#include "player_controller.hpp"
#include "scene.hpp"
#include "sandbox_scene_editor_state.hpp"

namespace prune {

    class SandboxScene final : public Scene {
    public:
        SandboxScene(int window_width, int window_height);

        void on_enter() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;
        void draw_viewport_panel() override;
        void draw_outline_panel() override;
        void draw_object_panel() override;
        void draw_debug_ui() override;

    private:
        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        [[nodiscard]] static GameObject create_player();
        [[nodiscard]] static GameObject create_initial_block();

        [[nodiscard]] Transform next_block_spawn_position() const noexcept;
        [[nodiscard]] std::string make_unique_name(std::string desired, GameObjectId ignore_id) const;

        GameObjectId create_block(float x, float y);
        [[nodiscard]] float random_color_component();

        // Game logic
        void update_game(float dt, const Input& input);
        void update_player(float dt, const Input& input);
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

        void draw_object_list_ui();

        GameObjectManager m_objects;
        PlayerController m_player_controller;

        GameObjectId m_player_id = kInvalidGameObjectId;

        SandboxEditorState m_editor_state;

        // Random number generation
        std::mt19937 m_rng{std::random_device{}()};
        std::uniform_real_distribution<float> m_color_dist{0.2f, 1.0f};

        int m_window_width = 0;
        int m_window_height = 0;
    };
}