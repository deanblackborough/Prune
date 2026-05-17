#pragma once

#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/scene_shell.hpp"
#include "prune/scene/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter_state.hpp"
#include "prune/tooling/simple_shooter.hpp"

namespace prune {

    class SimpleShooterScene : public SceneShell {
    public:
        SimpleShooterScene(int window_width, int window_height);
        SimpleShooterScene(const SimpleShooterScene&) = delete;
        SimpleShooterScene& operator=(const SimpleShooterScene&) = delete;

        void new_scene() override;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Simple Shooter"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Simple Shooter"; }
        void draw_scene_tools(bool& open) override;
        void draw_scene_inspector(GameObject& selected) override;

        SimpleShooterOptions& get_simple_shooter_options() noexcept;
        const SimpleShooterOptions& get_simple_shooter_options() const noexcept;

        SimpleShooterState& get_simple_shooter_state() noexcept;
        const SimpleShooterState& get_simple_shooter_state() const noexcept;

        void reset_simple_shooter();

        [[nodiscard]] int bullet_count() const noexcept;

        [[nodiscard]] GameObject* enemy_object() noexcept;
        [[nodiscard]] const GameObject* enemy_object() const noexcept;

    protected:
        void update_scene(float dt, const Input& input) override;
        [[nodiscard]] GameObject* follow_target() noexcept override;
        void on_scene_exit() override;
        void render_scene_overlay(SDL_Renderer* renderer) override;

    private:
        void reset_runtime_state();
        void restore_defaults();

        GameObjectId create_block_at_view_center();

        [[nodiscard]] Transform find_block_spawn_position(const GameObject& block) const;
        [[nodiscard]] bool is_space_free(const GameObject& candidate) const noexcept;

        void draw_player_facing_indicator(SDL_Renderer* renderer) const;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        SimpleShooterState m_simple_shooter_state;
        SimpleShooterBehaviour m_simple_shooter;
        SimpleShooter m_simple_shooter_tools;
    };
}