#pragma once

#include <span>
#include <string>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer/platformer_behaviour.hpp"
#include "prune/scene/platformer/platformer_state.hpp"
#include "prune/scene/world_scene.hpp"

namespace prune {

  class PlatformerScene : public WorldScene {
  public:
    PlatformerScene(int window_width, int window_height,
                    GridOptions& grid_options);

    void on_exit() override;

    void new_scene() override;

    [[nodiscard]] std::string_view default_file_path() const noexcept override;
    [[nodiscard]] std::string_view scene_type_id() const noexcept override;
    [[nodiscard]] std::string_view scene_name() const noexcept override {
      return "Platformer";
    }
    [[nodiscard]] std::string_view scene_tools_label() const noexcept override {
      return "Platformer";
    }
    [[nodiscard]] ObjectConcept
    object_concept_for(const GameObject& object) const override;
    [[nodiscard]] std::span<const SceneCreationAction>
    scene_creation_actions() const noexcept override;
    GameObjectId create_scene_object(std::string_view action_id) override;

    // Exposed for PlatformerEditor, which owns the ImGui-facing tools and
    // inspector panels. The scene still owns the data; the editor just binds
    // widgets to it.
    [[nodiscard]] PlatformerState& platformer_state() noexcept {
      return m_platformer_state;
    }

  protected:
    void update_runtime(WorldState& world, float dt, const Input& input,
                        bool keyboard_input_enabled) override;
    void restart_runtime() override;
    void set_runtime_paused(bool paused) noexcept override;
    [[nodiscard]] bool is_runtime_paused() const noexcept override;
    void save_scene_data(YAML::Node& root) const override;
    [[nodiscard]] bool load_scene_data(const YAML::Node& root,
                                       std::string& error) override;
    void apply_scene_settings_snapshot(const YAML::Node& node) override;
    [[nodiscard]] bool restore_loaded_scene(SceneState& state,
                                            std::string& error) override;
    [[nodiscard]] GameObject* game_camera_target() noexcept override;
    void establish_game_camera() override;

  private:
    void reset_runtime_state();
    void restore_defaults();
    [[nodiscard]] GameObject* player_object() noexcept;
    [[nodiscard]] const GameObject* player_object() const noexcept;

    [[nodiscard]] GameObjectId add_platform_at_view_center();
    [[nodiscard]] GameObjectId add_hazard_at_view_center();

    PlatformerState m_platformer_state;
    PlatformerBehaviour m_platformer;
  };

} // namespace prune
