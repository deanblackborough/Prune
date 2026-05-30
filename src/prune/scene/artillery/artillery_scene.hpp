#pragma once

#include <span>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include "prune/scene/artillery/artillery_behaviour.hpp"
#include "prune/scene/artillery/artillery_state.hpp"
#include "prune/scene/world_scene.hpp"
#include "prune/tooling/artillery/artillery.hpp"

namespace prune {

    class ArtilleryScene : public WorldScene {
    public:
        ArtilleryScene(int window_width, int window_height);

        void on_enter() override;
        void on_exit() override;
        void new_scene() override;

        [[nodiscard]] std::string_view default_file_path() const noexcept override;
        [[nodiscard]] std::string_view scene_type_id() const noexcept override;
        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Artillery"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Artillery"; }
        [[nodiscard]] ObjectConcept object_concept_for(const GameObject& object) const override;
        [[nodiscard]] std::span<const SceneCreationAction> scene_creation_actions() const noexcept override;
        GameObjectId create_scene_object(std::string_view action_id) override;
        void draw_scene_tools(bool& open) override;
        void draw_scene_inspector(GameObject& selected) override;

    protected:
        void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) override;
        void save_scene_data(YAML::Node& root) const override;
        [[nodiscard]] bool load_scene_data(const YAML::Node& root, std::string& error) override;
        [[nodiscard]] bool restore_loaded_scene(SceneState& state, std::string& error) override;
        void render_overlay(SDL_Renderer* renderer) override;

    private:
        void reset_runtime_state();
        void restore_defaults();
        [[nodiscard]] GameObjectId add_terrain_line_at_view_center();
        [[nodiscard]] GameObject* current_tank() noexcept;
        [[nodiscard]] const GameObject* current_tank() const noexcept;

        ArtilleryState m_artillery_state;
        ArtilleryBehaviour m_artillery;
        Artillery m_artillery_tools;
    };

}
