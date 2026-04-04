#pragma once

#include "game_object_manager.hpp"
#include "player_controller.hpp"
#include "scene.hpp"

#include <cstddef>

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

        GameObjectManager m_objects;
        PlayerController m_player_controller;

        std::size_t m_player_index = 0;

        int m_window_width = 0;
        int m_window_height = 0;
    };

} // namespace prune