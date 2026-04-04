#pragma once

#include "game_object.hpp"
#include "game_object_manager.hpp"
#include "player_controller.hpp"
#include "scene.hpp"

namespace prune {

    class SandboxScene final : public Scene {
    public:
        SandboxScene(int window_width, int window_height);

        void on_enter() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;
        void draw_inspector_ui() override;
        void draw_debug_ui() override;

    private:
        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        [[nodiscard]] bool is_overlapping(const GameObject& a, const GameObject& b) const noexcept;
        [[nodiscard]] Transform next_block_spawn_position() const noexcept;

        GameObjectId create_block(float x, float y);

        void resolve_player_collisions(GameObject& player);
        void draw_object_list_ui();
        void draw_selected_object_ui();

        GameObjectManager m_objects;
        PlayerController m_player_controller;

        GameObjectId m_player_id = kInvalidGameObjectId;

        int m_window_width = 0;
        int m_window_height = 0;
    };

} // namespace prune