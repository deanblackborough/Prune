#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include "prune/core/defaults.hpp"
#include "prune/scene/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter_state.hpp"
#include "prune/scene/world_scene.hpp"
#include "prune/tooling/simple_shooter.hpp"

namespace prune {

    class SimpleShooterScene : public WorldScene {
    public:
        SimpleShooterScene(int window_width, int window_height);
        SimpleShooterScene(const SimpleShooterScene&) = delete;
        SimpleShooterScene& operator=(const SimpleShooterScene&) = delete;
        void on_enter() override;
        void on_exit() override;

        void new_scene() override;
        [[nodiscard]] SceneState& get_state() noexcept;
        [[nodiscard]] const SceneState& get_state() const noexcept;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;
        [[nodiscard]] std::string_view default_file_path() const noexcept override { return k_default_simple_shooter_scene_file_path; }

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Simple Shooter"; }
        [[nodiscard]] std::string object_role_label(const GameObject& object) const override;
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Simple Shooter"; }
        void draw_scene_tools(bool& open) override;

        void draw_scene_inspector(GameObject& selected) override;

        SimpleShooterOptions& get_simple_shooter_options() noexcept;
        const SimpleShooterOptions& get_simple_shooter_options() const noexcept;

        SimpleShooterState& get_simple_shooter_state() noexcept;
        const SimpleShooterState& get_simple_shooter_state() const noexcept;

        void reset_simple_shooter();

        [[nodiscard]] int projectile_count() const noexcept;

        [[nodiscard]] GameObject* enemy_object() noexcept;
        [[nodiscard]] const GameObject* enemy_object() const noexcept;

    protected:
        void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) override;
        [[nodiscard]] GameObject* game_camera_target() noexcept override;
        void render_overlay(SDL_Renderer* renderer) override;

    private:
        void reset_runtime_state();
        void restore_defaults();

        GameObjectId create_wall_at_view_center();

        [[nodiscard]] Transform find_wall_spawn_position(const GameObject& wall) const;
        [[nodiscard]] bool is_space_free(const GameObject& candidate) const noexcept;

        void draw_player_facing_indicator(SDL_Renderer* renderer) const;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        SimpleShooterState m_simple_shooter_state;

        SimpleShooterBehaviour m_simple_shooter;
        SimpleShooter m_simple_shooter_tools;
    };
}