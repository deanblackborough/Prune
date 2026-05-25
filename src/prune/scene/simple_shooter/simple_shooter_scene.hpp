#pragma once

#include <span>
#include <string>
#include <string_view>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "prune/scene/simple_shooter/simple_shooter_behaviour.hpp"
#include "prune/scene/simple_shooter/simple_shooter_state.hpp"
#include "prune/scene/world_scene.hpp"
#include "prune/tooling/simple_shooter/simple_shooter.hpp"

namespace prune {

    class SimpleShooterScene : public WorldScene {
    public:
        SimpleShooterScene(int window_width, int window_height);
        SimpleShooterScene(const SimpleShooterScene&) = delete;
        SimpleShooterScene& operator=(const SimpleShooterScene&) = delete;
        void on_enter() override;
        void on_exit() override;

        void new_scene() override;

        [[nodiscard]] std::string_view default_file_path() const noexcept override;
        [[nodiscard]] std::string_view scene_type_id() const noexcept override;
        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Simple Shooter"; }
        [[nodiscard]] ObjectConcept object_concept_for(const GameObject& object) const override;
        [[nodiscard]] std::span<const SceneCreationAction> scene_creation_actions() const noexcept override;
        GameObjectId create_scene_object(std::string_view action_id) override;
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Simple Shooter"; }
        void draw_scene_tools(bool& open) override;

        void draw_scene_inspector(GameObject& selected) override;

    protected:
        void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) override;
        void save_scene_data(YAML::Node& root) const override;
        [[nodiscard]] bool load_scene_data(const YAML::Node& root, std::string& error) override;
        [[nodiscard]] bool restore_loaded_scene(SceneState& state, std::string& error) override;
        [[nodiscard]] GameObject* game_camera_target() noexcept override;
        void render_overlay(SDL_Renderer* renderer) override;

    private:
        void reset_runtime_state();
        void restore_defaults();

        GameObjectId create_wall_at_view_center();

        void draw_player_facing_indicator(SDL_Renderer* renderer) const;

        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        SimpleShooterState m_simple_shooter_state;

        SimpleShooterBehaviour m_simple_shooter;
        SimpleShooter m_simple_shooter_tools;
    };
}
