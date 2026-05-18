#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/scene.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter_state.hpp"
#include "prune/tooling/simple_shooter.hpp"

namespace prune {

    class SimpleShooterScene : public Scene {
    public:
        SimpleShooterScene(int window_width, int window_height);
        SimpleShooterScene(const SimpleShooterScene&) = delete;
        SimpleShooterScene& operator=(const SimpleShooterScene&) = delete;
        void on_enter() override;
        void on_exit() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;

        void new_scene() override;
        [[nodiscard]] SceneState& get_state() noexcept;
        [[nodiscard]] const SceneState& get_state() const noexcept;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;

        void set_viewport(const SceneViewport& viewport) noexcept override;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept override { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept override { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept override { return m_state.viewport.height; }

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Simple Shooter"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Simple Shooter"; }
        void draw_scene_tools(bool& open) override;

        GameObjectManager& get_object_manager() override;
        GridOptions& get_grid_options() override;
        SceneOptions& get_scene_options() override;

        void draw_scene_inspector(GameObject& selected) override;

        SceneCamera& get_camera() noexcept override;
        [[nodiscard]] const SceneCamera& get_camera() const noexcept override;

        SimpleShooterOptions& get_simple_shooter_options() noexcept;
        const SimpleShooterOptions& get_simple_shooter_options() const noexcept;

        SimpleShooterState& get_simple_shooter_state() noexcept;
        const SimpleShooterState& get_simple_shooter_state() const noexcept;

        void reset_simple_shooter();

        [[nodiscard]] int bullet_count() const noexcept;

        [[nodiscard]] GameObject* enemy_object() noexcept;
        [[nodiscard]] const GameObject* enemy_object() const noexcept;

    private:
        void reset_runtime_state();
        void restore_defaults();

        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        GameObjectId create_block_at_view_center();

        [[nodiscard]] Transform find_block_spawn_position(const GameObject& block) const;
        [[nodiscard]] bool is_space_free(const GameObject& candidate) const noexcept;

        void draw_player_facing_indicator(SDL_Renderer* renderer) const;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        SceneState m_state;
        SimpleShooterState m_simple_shooter_state;

        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
        SimpleShooterBehaviour m_simple_shooter;
        SimpleShooter m_simple_shooter_tools;
    };
}