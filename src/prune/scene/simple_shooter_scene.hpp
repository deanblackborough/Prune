#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/simple_shooter_behaviour.hpp"

namespace prune {

    class SimpleShooterScene {
    public:
        SimpleShooterScene(int window_width, int window_height);
        SimpleShooterScene(const SimpleShooterScene&) = delete;
        SimpleShooterScene& operator=(const SimpleShooterScene&) = delete;
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

        SceneCamera& get_camera() noexcept;
        const SceneCamera& get_camera() const noexcept;

        SimpleShooterOptions& get_simple_shooter_options() noexcept;
        const SimpleShooterOptions& get_simple_shooter_options() const noexcept;

        void reset_simple_shooter();

        [[nodiscard]] int bullet_count() const noexcept;

        [[nodiscard]] GameObject* enemy_object() noexcept;
        [[nodiscard]] const GameObject* enemy_object() const noexcept;

    private:
        void reset_runtime_state();
        void restore_defaults();

        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        SceneState m_state;
        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
        SimpleShooterBehaviour m_simple_shooter;
    };
}