#pragma once

#include <span>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include "prune/scene/artillery/artillery_behaviour.hpp"
#include "prune/scene/artillery/artillery_state.hpp"
#include "prune/scene/world_scene.hpp"

namespace prune {

  class ArtilleryScene : public WorldScene {
  public:
    ArtilleryScene(int window_width, int window_height,
                   GridOptions& grid_options);

    void on_exit() override;
    void new_scene() override;

    [[nodiscard]] std::string_view default_file_path() const noexcept override;
    [[nodiscard]] std::string_view scene_type_id() const noexcept override;
    [[nodiscard]] std::string_view scene_name() const noexcept override {
      return "Artillery";
    }
    [[nodiscard]] std::string_view scene_tools_label() const noexcept override {
      return "Artillery";
    }
    [[nodiscard]] ObjectConcept
    object_concept_for(const GameObject& object) const override;
    [[nodiscard]] std::span<const SceneCreationAction>
    scene_creation_actions() const noexcept override;
    GameObjectId create_scene_object(std::string_view action_id) override;

    // Exposed for ArtilleryEditor, which owns the ImGui-facing tools and
    // inspector panels. The scene still owns the data; the editor just binds
    // widgets to it.
    [[nodiscard]] ArtilleryState& artillery_state() noexcept {
      return m_artillery_state;
    }

  protected:
    void update_runtime(WorldState& world, float dt, const Input& input,
                        bool keyboard_input_enabled) override;
    void restart_runtime() override;
    void save_scene_data(YAML::Node& root) const override;
    [[nodiscard]] bool load_scene_data(const YAML::Node& root,
                                       std::string& error) override;
    void apply_scene_settings_snapshot(const YAML::Node& node) override;
    [[nodiscard]] bool restore_loaded_scene(SceneState& state,
                                            std::string& error) override;
    void render_overlay(SDL_Renderer* renderer) override;
    void establish_game_camera() override;

  private:
    void reset_runtime_state();
    void restore_defaults();
    [[nodiscard]] GameObjectId add_terrain_line_at_view_center();
    [[nodiscard]] GameObject* current_tank() noexcept;
    [[nodiscard]] const GameObject* current_tank() const noexcept;

    ArtilleryState m_artillery_state;
    ArtilleryBehaviour m_artillery;
  };

} // namespace prune
