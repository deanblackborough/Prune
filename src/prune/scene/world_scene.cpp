#include "prune/scene/world_scene.hpp"

#include <cmath>
#include <fstream>
#include <string>
#include <utility>

#include "prune/core/overloaded.hpp"
#include "prune/scene/collision.hpp"
#include "prune/scene/scene_serializer.hpp"

namespace prune {

  WorldScene::WorldScene(GridOptions& grid_options) noexcept
      : m_grid_options(&grid_options) {}

  void WorldScene::update(float dt, const Input& input) {
    update_runtime(m_state.world, dt, input, scene_keyboard_input_enabled());
    m_state.editor.selection.sanitize(m_state.world.objects);
    m_camera.update_game_camera(m_state.editor.viewport, game_camera_target());
  }

  void WorldScene::update_editor(float dt, const Input& input) {
    m_interaction.update(*this, m_state, m_camera, *m_grid_options, dt, input);
    m_camera.update_game_camera(m_state.editor.viewport, game_camera_target());
  }

  void WorldScene::render(SDL_Renderer* renderer) {
    m_renderer.render(renderer, *this, m_state, m_camera, *m_grid_options);
    render_overlay(renderer);
  }

  void WorldScene::on_enter() {
    on_scene_enter();
    m_authored_store.capture(m_state.world.objects);
    enter_stopped_state();
  }

  void WorldScene::enter_stopped_state() {
    const std::vector<GameObjectId> selected_ids(
        m_state.editor.selection.selected_ids().begin(),
        m_state.editor.selection.selected_ids().end());

    m_state.world.objects = m_authored_store.objects();
    m_state.editor.selection.select_many(m_state.world.objects, selected_ids);
    m_state.world.events.clear();
    m_state.editor.drag_state = {};

    restart_runtime();
    set_runtime_paused(true);
    m_stopped = true;
    m_camera.update_game_camera(m_state.editor.viewport, game_camera_target());
  }

  void WorldScene::reset_runtime() {
    enter_stopped_state();
    play_runtime();
  }

  void WorldScene::pause_runtime() noexcept { set_runtime_paused(true); }

  void WorldScene::play_runtime() noexcept {
    set_runtime_paused(false);
    m_stopped = false;
  }

  bool WorldScene::runtime_paused() const noexcept {
    return is_runtime_paused();
  }

  void WorldScene::stop_runtime() { enter_stopped_state(); }

  bool WorldScene::runtime_stopped() const noexcept { return m_stopped; }

  bool WorldScene::execute_scene_creation_action(std::string_view action_id) {
    for (const SceneCreationAction& action : scene_creation_actions()) {
      if (action.id != action_id) {
        continue;
      }

      const GameObjectId created_id = create_scene_object(action.id);
      if (created_id == k_invalid_game_object_id) {
        return false;
      }

      const GameObject* created = m_state.world.objects.get_by_id(created_id);
      if (!created) {
        return false;
      }

      record_editor_command(make_object_create(*created, action.label));
      return true;
    }

    return false;
  }

  bool WorldScene::save_to_file(std::string_view path, std::string& error) {
    try {
      YAML::Node root;
      root["scene_type"] = std::string(scene_type_id());

      SceneState authored_state = m_state;
      authored_state.world.objects = m_authored_store.objects();
      SceneSerializer::save_to_node(authored_state, root);
      save_scene_data(root);

      std::ofstream output{std::string(path)};

      if (!output.is_open()) {
        error = "Could not open save file for writing.";
        return false;
      }

      output << root;

      if (!output) {
        error = "Failed to write scene data to file.";
        return false;
      }

      m_state.editor.dirty = false;
      return true;
    } catch (const YAML::Exception& ex) {
      error = ex.what();
      return false;
    } catch (const std::exception& ex) {
      error = ex.what();
      return false;
    }
  }

  bool WorldScene::load_from_file(std::string_view path, std::string& error) {
    try {
      const YAML::Node root = YAML::LoadFile(std::string(path));

      if (!root["scene_type"] ||
          root["scene_type"].as<std::string>() != scene_type_id()) {
        error = "Save file is not a " + std::string(scene_name()) + " scene.";
        return false;
      }

      SceneState loaded_state = m_state;
      loaded_state.world.events.clear();

      if (!SceneSerializer::load_from_node(loaded_state, root, error)) {
        return false;
      }

      if (!load_scene_data(root, error)) {
        return false;
      }

      if (!restore_loaded_scene(loaded_state, error)) {
        return false;
      }

      m_state = std::move(loaded_state);
      m_state.world.events.clear();
      m_state.editor.drag_state = {};
      m_state.editor.editor_commands.clear();
      m_state.editor.dirty = false;

      sanitize_loaded_selection();
      m_camera.reset();
      establish_game_camera();
      m_authored_store.capture(m_state.world.objects);

      return true;
    } catch (const YAML::Exception& ex) {
      error = ex.what();
      return false;
    } catch (const std::exception& ex) {
      error = ex.what();
      return false;
    }
  }

  void WorldScene::sanitize_loaded_selection() noexcept {
    if (GameObject* selected =
            m_state.editor.selection.selected_object(m_state.world.objects);
        selected != nullptr && object_is_selectable(*selected)) {
      return;
    }

    for (const auto& object : m_state.world.objects.objects()) {
      if (object_is_selectable(object)) {
        m_state.editor.selection.set_selected_id(m_state.world.objects,
                                                 object.identity.id);
        return;
      }
    }

    m_state.editor.selection.set_selected_id(m_state.world.objects,
                                             k_invalid_game_object_id);
  }

  void WorldScene::set_viewport(const SceneViewport& viewport) noexcept {
    m_state.editor.viewport = viewport;
  }

  GameObjectManager& WorldScene::get_object_manager() {
    return m_state.world.objects;
  }

  EditorSelection& WorldScene::selection() noexcept {
    return m_state.editor.selection;
  }

  const EditorSelection& WorldScene::selection() const noexcept {
    return m_state.editor.selection;
  }

  void WorldScene::record_editor_command(EditorCommand command) {
    m_authored_store.apply(command, true);

    if (command.makes_dirty) {
      m_state.editor.dirty = true;
    }

    m_state.editor.editor_commands.record(std::move(command));
  }

  const EditorCommandHistory&
  WorldScene::editor_command_history() const noexcept {
    return m_state.editor.editor_commands;
  }

  bool WorldScene::undo_editor_command() {
    const EditorCommand* command =
        m_state.editor.editor_commands.undo_command();
    if (!command) {
      return false;
    }

    apply_editor_command(*command, false);
    m_authored_store.apply(*command, false);
    if (command->makes_dirty) {
      m_state.editor.dirty = true;
    }
    return true;
  }

  bool WorldScene::redo_editor_command() {
    const EditorCommand* command =
        m_state.editor.editor_commands.redo_command();
    if (!command) {
      return false;
    }

    apply_editor_command(*command, true);
    m_authored_store.apply(*command, true);
    if (command->makes_dirty) {
      m_state.editor.dirty = true;
    }
    return true;
  }

  SceneOptions& WorldScene::get_scene_options() {
    return m_state.editor.scene_options;
  }

  std::span<const SceneEvent>
  WorldScene::pending_scene_events() const noexcept {
    return m_state.world.events.pending();
  }

  void WorldScene::clear_scene_events() noexcept {
    m_state.world.events.clear();
  }

  EditorTool WorldScene::current_editor_tool() const noexcept {
    return m_state.editor.editor_tool;
  }

  void WorldScene::set_current_editor_tool(EditorTool tool) noexcept {
    m_state.editor.editor_tool = tool;
  }

  WorldSceneContext WorldScene::world_scene_context() noexcept {
    return WorldSceneContext{m_grid_options, &m_camera};
  }

  ConstWorldSceneContext WorldScene::world_scene_context() const noexcept {
    return ConstWorldSceneContext{m_grid_options, &m_camera};
  }

  void WorldScene::restore_object_snapshot(const GameObject& object,
                                           bool select_restored) {
    restore_object_in_id_order(m_state.world.objects, object);

    if (select_restored) {
      m_state.editor.selection.select(m_state.world.objects,
                                      object.identity.id);
    }
  }

  void
  WorldScene::restore_object_snapshots(const std::vector<GameObject>& objects) {
    std::vector<GameObjectId> restored_ids;
    restored_ids.reserve(objects.size());

    for (const GameObject& object : objects) {
      restore_object_snapshot(object, false);
      restored_ids.push_back(object.identity.id);
    }

    m_state.editor.selection.select_many(m_state.world.objects, restored_ids);
  }

  void WorldScene::apply_editor_command(const EditorCommand& command,
                                        bool use_after_state) {
    std::visit(overloaded{
                   [&](const ObjectCreate& payload) {
                     if (use_after_state) {
                       restore_object_snapshot(payload.object);
                     } else {
                       const GameObjectId id = payload.object.identity.id;
                       const std::size_t erased_index =
                           m_state.world.objects.index_of(id);
                       m_state.world.objects.remove_object(id);
                       m_state.editor.selection.object_removed(
                           m_state.world.objects, id, erased_index);
                     }
                   },
                   [&](const ObjectDelete& payload) {
                     if (use_after_state) {
                       const GameObjectId id = payload.object.identity.id;
                       const std::size_t erased_index =
                           m_state.world.objects.index_of(id);
                       m_state.world.objects.remove_object(id);
                       m_state.editor.selection.object_removed(
                           m_state.world.objects, id, erased_index);
                     } else {
                       restore_object_snapshot(payload.object);
                     }
                   },
                   [&](const ObjectsDelete& payload) {
                     if (use_after_state) {
                       for (const GameObject& object : payload.objects) {
                         const GameObjectId id = object.identity.id;
                         const std::size_t erased_index =
                             m_state.world.objects.index_of(id);
                         m_state.world.objects.remove_object(id);
                         m_state.editor.selection.object_removed(
                             m_state.world.objects, id, erased_index);
                       }

                       m_state.editor.selection.clear_selection();
                     } else {
                       restore_object_snapshots(payload.objects);
                     }
                   },
                   [&](const ObjectsChange& payload) {
                     restore_object_snapshots(use_after_state ? payload.after
                                                              : payload.before);
                   },
                   [&](const ViewportChange& payload) {
                     m_camera.editor() =
                         use_after_state ? payload.after : payload.before;
                     m_camera.activate_editor();
                   },
                   [&](const SceneSettingsChange& payload) {
                     apply_scene_settings_snapshot(
                         use_after_state ? payload.after : payload.before);
                   },
                   [&](const ObjectChange& payload) {
                     restore_object_snapshot(use_after_state ? payload.after
                                                             : payload.before);
                   },
               },
               command.payload);
  }

  bool WorldScene::scene_keyboard_input_enabled() const noexcept {
    return m_state.editor.viewport.keyboard_input_enabled &&
           m_state.editor.viewport.has_area();
  }

  bool WorldScene::scene_mouse_input_enabled() const noexcept {
    return m_state.editor.viewport.hovered &&
           m_state.editor.viewport.has_area();
  }

  Transform WorldScene::view_center_spawn_position(int width,
                                                   int height) const {
    const Camera& camera = m_camera.active();

    const float view_center_x =
        camera.x +
        (static_cast<float>(m_state.editor.viewport.width) / camera.zoom) *
            0.5f;
    const float view_center_y =
        camera.y +
        (static_cast<float>(m_state.editor.viewport.height) / camera.zoom) *
            0.5f;

    Transform transform{};
    transform.x = view_center_x - (static_cast<float>(width) * 0.5f);
    transform.y = view_center_y - (static_cast<float>(height) * 0.5f);

    if (!m_grid_options->snap_to_grid || m_grid_options->grid_size <= 0) {
      return transform;
    }

    const float grid = static_cast<float>(m_grid_options->grid_size);
    transform.x = std::floor(transform.x / grid) * grid;
    transform.y = std::floor(transform.y / grid) * grid;
    return transform;
  }

  Transform WorldScene::first_free_view_center_spawn_position(
      const GameObject& object) const {
    const int grid_size = m_grid_options->grid_size > 0
                              ? m_grid_options->grid_size
                              : object.size.width;

    const Transform base =
        view_center_spawn_position(object.size.width, object.size.height);

    constexpr int max_radius = 20;

    for (int radius = 0; radius <= max_radius; ++radius) {
      for (int y = -radius; y <= radius; ++y) {
        for (int x = -radius; x <= radius; ++x) {
          if (std::abs(x) != radius && std::abs(y) != radius) {
            continue;
          }

          GameObject candidate = object;
          candidate.transform.x = base.x + static_cast<float>(x * grid_size);
          candidate.transform.y = base.y + static_cast<float>(y * grid_size);

          if (is_space_free(candidate)) {
            return candidate.transform;
          }
        }
      }
    }

    return base;
  }

  bool WorldScene::is_space_free(const GameObject& candidate) const noexcept {
    for (const auto& object : m_state.world.objects.objects()) {
      if (!object.lifecycle.active) {
        continue;
      }

      if (!object.runtime.persistent) {
        continue;
      }

      if (collision::is_overlapping(candidate, object)) {
        return false;
      }
    }

    return true;
  }
} // namespace prune
