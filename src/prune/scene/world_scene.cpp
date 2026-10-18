#include "prune/scene/world_scene.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>

#include "imgui.h"

#include "prune/scene/collision.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

  namespace {
    template <class... Visitors> struct overloaded : Visitors... {
      using Visitors::operator()...;
    };
    template <class... Visitors>
    overloaded(Visitors...) -> overloaded<Visitors...>;

    void restore_object_in_id_order(GameObjectManager& objects,
                                    const GameObject& object) {
      if (GameObject* existing = objects.get_by_id(object.identity.id)) {
        *existing = object;
        return;
      }

      objects.add_loaded_object(object);
      std::ranges::stable_sort(
          objects.objects(), {},
          [](const GameObject& candidate) { return candidate.identity.id; });
    }
  } // namespace

  WorldScene::WorldScene(GridOptions& grid_options) noexcept
      : m_grid_options(&grid_options) {}

  void WorldScene::update(float dt, const Input& input) {
    update_runtime(dt, input, scene_keyboard_input_enabled());
    m_camera.update_game_camera(m_state.viewport, game_camera_target());
  }

  void WorldScene::update_editor(float dt, const Input& input) {
    m_interaction.update(*this, m_state, m_camera, *m_grid_options, dt, input);
    m_camera.update_game_camera(m_state.viewport, game_camera_target());
  }

  void WorldScene::render(SDL_Renderer* renderer) {
    m_renderer.render(renderer, *this, m_state, m_camera, *m_grid_options);
    render_overlay(renderer);
  }

  void WorldScene::on_enter() {
    capture_authored_objects();
    on_scene_enter();
    enter_stopped_state();
  }

  void WorldScene::enter_stopped_state() {
    const std::vector<GameObjectId> selected_ids(
        m_state.objects.selected_ids().begin(),
        m_state.objects.selected_ids().end());

    m_state.objects = m_authored_objects;
    m_state.objects.select_many(selected_ids);
    m_state.events.clear();
    m_state.drag_state = {};

    restart_runtime();
    set_runtime_paused(true);
    m_stopped = true;
    m_camera.update_game_camera(m_state.viewport, game_camera_target());
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

  void WorldScene::draw_viewport_overlays() {
    if (!m_state.viewport.has_area()) {
      return;
    }

    if (!m_state.scene_options.debug_overlays.show_role_labels) {
      return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const Camera& camera = m_camera.active();
    const float zoom = std::max(camera.zoom, 0.01f);
    const ImU32 label_colour = IM_COL32(230, 220, 245, 230);
    const ImU32 label_shadow_colour = IM_COL32(18, 14, 24, 220);

    for (const auto& object : m_state.objects.objects()) {
      if (!object.lifecycle.active || !object.render.visible) {
        continue;
      }

      const std::string label = object_role_label(object);
      if (label.empty()) {
        continue;
      }

      const float screen_x = static_cast<float>(m_state.viewport.screen_x) +
                             ((object.transform.x - camera.x) * zoom);

      const float screen_y = static_cast<float>(m_state.viewport.screen_y) +
                             ((object.transform.y - camera.y) * zoom) - 18.0f;

      const ImVec2 pos{screen_x, screen_y};
      draw_list->AddText(ImVec2(pos.x + 1.0f, pos.y + 1.0f),
                         label_shadow_colour, label.c_str());
      draw_list->AddText(pos, label_colour, label.c_str());
    }
  }

  bool WorldScene::execute_scene_creation_action(std::string_view action_id) {
    for (const SceneCreationAction& action : scene_creation_actions()) {
      if (action.id != action_id) {
        continue;
      }

      const GameObjectId created_id = create_scene_object(action.id);
      if (created_id == k_invalid_game_object_id) {
        return false;
      }

      const GameObject* created = m_state.objects.get_by_id(created_id);
      if (!created) {
        return false;
      }

      record_editor_command(make_object_create(*created, action.label));
      return true;
    }

    return false;
  }

  void WorldScene::draw_debug_tools() {
    if (!tooling::imgui::layout::collapsing_header("Debug")) {
      return;
    }

    DebugOverlayOptions& overlays = m_state.scene_options.debug_overlays;

    if (tooling::imgui::property_table::begin("Debug")) {
      tooling::imgui::property_table::checkbox("Collision bounds",
                                               "###collision_bounds",
                                               overlays.show_collision_bounds);
      tooling::imgui::property_table::checkbox("Runtime object markers",
                                               "###runtime_markers",
                                               overlays.show_runtime_markers);
      tooling::imgui::property_table::checkbox("Scene role labels",
                                               "###scene_role_labels",
                                               overlays.show_role_labels);
      tooling::imgui::property_table::end();
    }
  }

  bool WorldScene::save_to_file(std::string_view path, std::string& error) {
    try {
      YAML::Node root;
      root["scene_type"] = std::string(scene_type_id());

      SceneState authored_state = m_state;
      authored_state.objects = m_authored_objects;
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

      m_state.dirty = false;
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
      loaded_state.events.clear();

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
      m_state.events.clear();
      m_state.drag_state = {};
      m_state.editor_commands.clear();
      m_state.dirty = false;

      sanitize_loaded_selection();
      m_camera.reset();
      establish_game_camera();
      capture_authored_objects();

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
    if (GameObject* selected = m_state.objects.selected_object();
        selected != nullptr && object_is_selectable(*selected)) {
      return;
    }

    for (const auto& object : m_state.objects.objects()) {
      if (object_is_selectable(object)) {
        m_state.objects.set_selected_id(object.identity.id);
        return;
      }
    }

    m_state.objects.set_selected_id(k_invalid_game_object_id);
  }

  void WorldScene::set_viewport(const SceneViewport& viewport) noexcept {
    m_state.viewport = viewport;
  }

  GameObjectManager& WorldScene::get_object_manager() {
    return m_state.objects;
  }

  void WorldScene::record_editor_command(EditorCommand command) {
    apply_editor_command_to_authored_objects(command, true);

    if (command.makes_dirty) {
      m_state.dirty = true;
    }

    m_state.editor_commands.record(std::move(command));
  }

  const EditorCommandHistory&
  WorldScene::editor_command_history() const noexcept {
    return m_state.editor_commands;
  }

  bool WorldScene::undo_editor_command() {
    const EditorCommand* command = m_state.editor_commands.undo_command();
    if (!command) {
      return false;
    }

    apply_editor_command(*command, false);
    apply_editor_command_to_authored_objects(*command, false);
    if (command->makes_dirty) {
      m_state.dirty = true;
    }
    return true;
  }

  bool WorldScene::redo_editor_command() {
    const EditorCommand* command = m_state.editor_commands.redo_command();
    if (!command) {
      return false;
    }

    apply_editor_command(*command, true);
    apply_editor_command_to_authored_objects(*command, true);
    if (command->makes_dirty) {
      m_state.dirty = true;
    }
    return true;
  }

  SceneOptions& WorldScene::get_scene_options() {
    return m_state.scene_options;
  }

  std::span<const SceneEvent>
  WorldScene::pending_scene_events() const noexcept {
    return m_state.events.pending();
  }

  void WorldScene::clear_scene_events() noexcept { m_state.events.clear(); }

  EditorTool WorldScene::current_editor_tool() const noexcept {
    return m_state.editor_tool;
  }

  void WorldScene::set_current_editor_tool(EditorTool tool) noexcept {
    m_state.editor_tool = tool;
  }

  WorldSceneContext WorldScene::world_scene_context() noexcept {
    return WorldSceneContext{m_grid_options, &m_camera};
  }

  ConstWorldSceneContext WorldScene::world_scene_context() const noexcept {
    return ConstWorldSceneContext{m_grid_options, &m_camera};
  }

  void WorldScene::capture_authored_objects() {
    m_authored_objects = m_state.objects;
    m_authored_objects.remove_runtime_objects();
  }

  void WorldScene::apply_editor_command_to_authored_objects(
      const EditorCommand& command, bool use_after_state) {
    const auto restore_authored = [this](const GameObject& object) {
      restore_object_in_id_order(m_authored_objects, object);
    };

    std::visit(
        overloaded{
            [&](const ObjectCreate& payload) {
              if (use_after_state) {
                restore_authored(payload.object);
              } else {
                m_authored_objects.remove_object(payload.object.identity.id);
              }
            },
            [&](const ObjectDelete& payload) {
              if (use_after_state) {
                m_authored_objects.remove_object(payload.object.identity.id);
              } else {
                restore_authored(payload.object);
              }
            },
            [&](const ObjectsDelete& payload) {
              if (use_after_state) {
                for (const GameObject& object : payload.objects) {
                  m_authored_objects.remove_object(object.identity.id);
                }
              } else {
                for (const GameObject& object : payload.objects) {
                  restore_authored(object);
                }
              }
            },
            [&](const ObjectsChange& payload) {
              for (const GameObject& object :
                   use_after_state ? payload.after : payload.before) {
                restore_authored(object);
              }
            },
            [](const ViewportChange&) {},
            [&](const ObjectChange& payload) {
              restore_authored(use_after_state ? payload.after
                                               : payload.before);
            },
        },
        command.payload);
  }

  void WorldScene::restore_object_snapshot(const GameObject& object,
                                           bool select_restored) {
    restore_object_in_id_order(m_state.objects, object);

    if (select_restored) {
      m_state.objects.select(object.identity.id);
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

    m_state.objects.select_many(restored_ids);
  }

  void WorldScene::apply_editor_command(const EditorCommand& command,
                                        bool use_after_state) {
    std::visit(
        overloaded{
            [&](const ObjectCreate& payload) {
              if (use_after_state) {
                restore_object_snapshot(payload.object);
              } else {
                m_state.objects.remove_object(payload.object.identity.id);
              }
            },
            [&](const ObjectDelete& payload) {
              if (use_after_state) {
                m_state.objects.remove_object(payload.object.identity.id);
              } else {
                restore_object_snapshot(payload.object);
              }
            },
            [&](const ObjectsDelete& payload) {
              if (use_after_state) {
                for (const GameObject& object : payload.objects) {
                  m_state.objects.remove_object(object.identity.id);
                }

                m_state.objects.clear_selection();
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
            [&](const ObjectChange& payload) {
              restore_object_snapshot(use_after_state ? payload.after
                                                      : payload.before);
            },
        },
        command.payload);
  }

  bool WorldScene::scene_keyboard_input_enabled() const noexcept {
    return m_state.viewport.keyboard_input_enabled &&
           m_state.viewport.has_area();
  }

  bool WorldScene::scene_mouse_input_enabled() const noexcept {
    return m_state.viewport.hovered && m_state.viewport.has_area();
  }

  Transform WorldScene::view_center_spawn_position(int width,
                                                   int height) const {
    const Camera& camera = m_camera.active();

    const float view_center_x =
        camera.x +
        (static_cast<float>(m_state.viewport.width) / camera.zoom) * 0.5f;
    const float view_center_y =
        camera.y +
        (static_cast<float>(m_state.viewport.height) / camera.zoom) * 0.5f;

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
    for (const auto& object : m_state.objects.objects()) {
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
