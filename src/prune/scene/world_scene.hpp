#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "prune/core/input.hpp"
#include "prune/editor/editor_command.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

  class WorldScene : public Scene {
  public:
    void on_enter() final;

    void update(float dt, const Input& input) final;
    void update_editor(float dt, const Input& input) final;
    void render(SDL_Renderer* renderer) final;
    void draw_viewport_overlays() final;

    void reset_runtime() final;
    void pause_runtime() noexcept final;
    void play_runtime() noexcept final;
    [[nodiscard]] bool runtime_paused() const noexcept final;

    [[nodiscard]] bool save_to_file(std::string_view path,
                                    std::string& error) final;
    [[nodiscard]] bool load_from_file(std::string_view path,
                                      std::string& error) final;
    [[nodiscard]] bool is_dirty() const noexcept final { return m_state.dirty; }

    void set_viewport(const SceneViewport& viewport) noexcept final;
    [[nodiscard]] const SceneViewport& get_viewport() const noexcept final {
      return m_state.viewport;
    }
    [[nodiscard]] int get_viewport_width() const noexcept final {
      return m_state.viewport.width;
    }
    [[nodiscard]] int get_viewport_height() const noexcept final {
      return m_state.viewport.height;
    }

    GameObjectManager& get_object_manager() final;
    void record_editor_command(EditorCommand command) final;
    [[nodiscard]] const EditorCommandHistory&
    editor_command_history() const noexcept final;
    bool undo_editor_command() final;
    bool redo_editor_command() final;
    SceneOptions& get_scene_options() final;
    [[nodiscard]] std::span<const SceneEvent>
    pending_scene_events() const noexcept final;
    void clear_scene_events() noexcept final;
    [[nodiscard]] EditorTool current_editor_tool() const noexcept final;
    void set_current_editor_tool(EditorTool tool) noexcept final;
    bool execute_scene_creation_action(std::string_view action_id) final;

    [[nodiscard]] WorldSceneContext world_scene_context() noexcept final;
    [[nodiscard]] ConstWorldSceneContext
    world_scene_context() const noexcept final;
    void bind_grid_options(GridOptions& grid_options) noexcept final;

  protected:
    WorldScene() = default;

    [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
    [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

    [[nodiscard]] Transform view_center_spawn_position(int width,
                                                       int height) const;
    [[nodiscard]] Transform
    first_free_view_center_spawn_position(const GameObject& object) const;
    [[nodiscard]] bool
    is_space_free(const GameObject& candidate) const noexcept;

    void draw_debug_tools();

    virtual void on_scene_enter() {}
    virtual void update_runtime(float dt, const Input& input,
                                bool keyboard_input_enabled) = 0;
    virtual void restart_runtime() = 0;
    virtual void set_runtime_paused(bool paused) noexcept = 0;
    [[nodiscard]] virtual bool is_runtime_paused() const noexcept = 0;
    virtual void save_scene_data(YAML::Node& root) const = 0;
    [[nodiscard]] virtual bool load_scene_data(const YAML::Node& root,
                                               std::string& error) = 0;
    [[nodiscard]] virtual bool restore_loaded_scene(SceneState& state,
                                                    std::string& error) = 0;

    [[nodiscard]] virtual GameObject* game_camera_target() noexcept {
      return nullptr;
    }
    virtual void render_overlay(SDL_Renderer*) {}
    virtual void establish_game_camera() {}

    SceneState m_state;
    GridOptions* m_grid_options = nullptr;
    SceneCamera m_camera;

  private:
    void sanitize_loaded_selection() noexcept;
    void capture_authored_objects();
    void normalize_editor_command(EditorCommand& command) const;
    void apply_editor_command_to_authored_objects(const EditorCommand& command,
                                                  bool use_after_state);
    void restore_object_snapshot(const GameObject& object,
                                 bool select_restored = true);
    void restore_object_snapshots(const std::vector<GameObject>& objects);
    void apply_editor_command(const EditorCommand& command,
                              bool use_after_state);

    SceneRenderer m_renderer;
    SceneInteraction m_interaction;
    GameObjectManager m_authored_objects;
  };
} // namespace prune
