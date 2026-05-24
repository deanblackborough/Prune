#pragma once

#include <string>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer_behaviour.hpp"
#include "prune/scene/platformer_state.hpp"
#include "prune/scene/world_scene.hpp"
#include "prune/tooling/platformer.hpp"

namespace prune {

    class PlatformerScene : public WorldScene {
    public:
        PlatformerScene(int window_width, int window_height);

        void on_enter() override;
        void on_exit() override;

        void new_scene() override;

        [[nodiscard]] std::string_view default_file_path() const noexcept override;
        [[nodiscard]] std::string_view scene_type_id() const noexcept override;
        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Platformer"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Platformer"; }
        [[nodiscard]] ObjectConcept object_concept_for(const GameObject& object) const override;
        void draw_scene_tools(bool& open) override;

        void draw_scene_inspector(GameObject& selected) override;

    protected:
        void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) override;
        void save_scene_data(YAML::Node& root) const override;
        [[nodiscard]] bool load_scene_data(const YAML::Node& root, std::string& error) override;
        [[nodiscard]] bool restore_loaded_scene(SceneState& state, std::string& error) override;
        [[nodiscard]] GameObject* game_camera_target() noexcept override;

    private:
        void reset_runtime_state();
        void restore_defaults();
        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        void add_platform_at_view_center();
        void add_hazard_at_view_center();

        PlatformerState m_platformer_state;
        PlatformerBehaviour m_platformer;
        Platformer m_platformer_tools;
    };

}
