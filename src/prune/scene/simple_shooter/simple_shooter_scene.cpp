#include <algorithm>
#include <array>
#include <cmath>
#include <string>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "prune/core/input.hpp"
#include "prune/scene/scene_factory.hpp"
#include "prune/scene/simple_shooter/simple_shooter_concepts.hpp"
#include "prune/scene/simple_shooter/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter/simple_shooter_ids.hpp"
#include "prune/scene/simple_shooter/simple_shooter_scene.hpp"
#include "prune/scene/simple_shooter/simple_shooter_serializer.hpp"

namespace prune {

  namespace {

    [[nodiscard]] GameObjectId first_object_id_for_kind(
        const SceneState& state,
        simple_shooter_concepts::ObjectKind kind) noexcept {
      for (const auto& object : state.objects.objects()) {
        if (simple_shooter_concepts::kind_for(object) == kind) {
          return object.identity.id;
        }
      }

      return k_invalid_game_object_id;
    }

    [[nodiscard]] bool
    object_has_kind(const SceneState& state, GameObjectId id,
                    simple_shooter_concepts::ObjectKind kind) noexcept {
      const GameObject* object = state.objects.get_by_id(id);
      return object && simple_shooter_concepts::kind_for(*object) == kind;
    }

    void restore_loaded_shooter_concepts(SceneState& state,
                                         SimpleShooterState& shooter_state) {
      if (GameObject* player =
              state.objects.get_by_id(shooter_state.player_id)) {
        player->editor.movable = true;
      }

      if (!object_has_kind(state, shooter_state.player_id,
                           simple_shooter_concepts::ObjectKind::Player)) {
        shooter_state.player_id = first_object_id_for_kind(
            state, simple_shooter_concepts::ObjectKind::Player);
      }

      if (shooter_state.player_id == k_invalid_game_object_id) {
        shooter_state.player_id = state.objects.create_object(
            simple_shooter_factory::create_player());
      }

      if (!object_has_kind(state, shooter_state.enemy_spawn_id,
                           simple_shooter_concepts::ObjectKind::Spawn)) {
        shooter_state.enemy_spawn_id = first_object_id_for_kind(
            state, simple_shooter_concepts::ObjectKind::Spawn);
      }

      if (shooter_state.enemy_spawn_id == k_invalid_game_object_id) {
        shooter_state.enemy_spawn_id = state.objects.create_object(
            simple_shooter_factory::create_enemy_spawn());
      }

      if (!object_has_kind(state, shooter_state.enemy_id,
                           simple_shooter_concepts::ObjectKind::Enemy)) {
        shooter_state.enemy_id = first_object_id_for_kind(
            state, simple_shooter_concepts::ObjectKind::Enemy);
      }

      if (shooter_state.enemy_id == k_invalid_game_object_id) {
        shooter_state.enemy_id =
            state.objects.create_object(simple_shooter_factory::create_enemy());
      }
    }
  } // namespace

  SimpleShooterScene::SimpleShooterScene(int window_width, int window_height,
                                         GridOptions& grid_options)
      : WorldScene(grid_options) {
    m_state.viewport.width = window_width;
    m_state.viewport.height = window_height;
  }

  void SimpleShooterScene::on_exit() {
    m_state.objects.clear();
    m_simple_shooter_state.player_id = k_invalid_game_object_id;
    m_simple_shooter_state.enemy_id = k_invalid_game_object_id;
    m_simple_shooter_state.enemy_spawn_id = k_invalid_game_object_id;
  }

  ObjectConcept
  SimpleShooterScene::object_concept_for(const GameObject& object) const {
    return simple_shooter_concepts::describe_object(object);
  }

  std::span<const SceneCreationAction>
  SimpleShooterScene::scene_creation_actions() const noexcept {
    static constexpr std::array<SceneCreationAction, 1> actions{
        {{"wall", "Wall"}}};

    return actions;
  }

  GameObjectId
  SimpleShooterScene::create_scene_object(std::string_view action_id) {
    if (action_id == "wall") {
      return create_wall_at_view_center();
    }

    return k_invalid_game_object_id;
  }

  void SimpleShooterScene::update_runtime(float dt, const Input& input,
                                          bool keyboard_input_enabled) {
    m_simple_shooter.update(m_state, m_camera, m_simple_shooter_state, dt,
                            input, keyboard_input_enabled);
  }

  void SimpleShooterScene::restart_runtime() {
    m_simple_shooter_state.runtime = {};
  }

  void SimpleShooterScene::set_runtime_paused(bool paused) noexcept {
    m_simple_shooter_state.runtime.paused = paused;
  }

  bool SimpleShooterScene::is_runtime_paused() const noexcept {
    return m_simple_shooter_state.runtime.paused;
  }

  GameObject* SimpleShooterScene::game_camera_target() noexcept {
    return player_object();
  }

  void SimpleShooterScene::render_overlay(SDL_Renderer* renderer) {
    draw_player_facing_indicator(renderer);
  }

  GameObject* SimpleShooterScene::player_object() noexcept {
    return m_state.objects.get_by_id(m_simple_shooter_state.player_id);
  }

  const GameObject* SimpleShooterScene::player_object() const noexcept {
    return m_state.objects.get_by_id(m_simple_shooter_state.player_id);
  }

  void SimpleShooterScene::reset_runtime_state() {
    m_state.objects.clear();
    m_simple_shooter_state.player_id = k_invalid_game_object_id;
    m_simple_shooter_state.enemy_id = k_invalid_game_object_id;
    m_simple_shooter_state.enemy_spawn_id = k_invalid_game_object_id;

    m_camera.reset();

    m_state.scene_options = {};
    m_state.drag_state = {};
    m_state.editor_tool = EditorTool::Select;
    m_state.editor_commands.clear();

    m_simple_shooter_state = {};
  }

  void SimpleShooterScene::restore_defaults() {
    reset_runtime_state();

    m_simple_shooter_state.player_id =
        m_state.objects.create_object(simple_shooter_factory::create_player());

    m_state.objects.create_object(simple_shooter_factory::create_wall(
        176.0f, 96.0f, 16, 64, "Central Wall"));
    m_state.objects.create_object(simple_shooter_factory::create_wall(
        80.0f, 192.0f, 96, 16, "Lower Cover"));
    m_state.objects.create_object(simple_shooter_factory::create_wall(
        224.0f, 64.0f, 96, 16, "Upper Cover"));

    m_simple_shooter_state.enemy_spawn_id = m_state.objects.create_object(
        simple_shooter_factory::create_enemy_spawn());

    m_simple_shooter_state.enemy_id =
        m_state.objects.create_object(simple_shooter_factory::create_enemy());

    m_simple_shooter.reset(m_state, m_simple_shooter_state);

    m_state.objects.select(m_simple_shooter_state.player_id);

    establish_game_camera();
  }

  void SimpleShooterScene::establish_game_camera() {
    m_camera.update_game_camera(m_state.viewport, player_object());
  }

  void SimpleShooterScene::new_scene() { restore_defaults(); }

  std::string_view SimpleShooterScene::default_file_path() const noexcept {
    if (const SceneDescriptor* descriptor =
            scene_descriptor_for(SceneType::SimpleShooter)) {
      return descriptor->default_file_path;
    }

    return {};
  }

  std::string_view SimpleShooterScene::scene_type_id() const noexcept {
    if (const SceneDescriptor* descriptor =
            scene_descriptor_for(SceneType::SimpleShooter)) {
      return descriptor->id;
    }

    return {};
  }

  void SimpleShooterScene::save_scene_data(YAML::Node& root) const {
    SimpleShooterSerializer::save_to_node(m_simple_shooter_state, root);
  }

  bool SimpleShooterScene::load_scene_data(const YAML::Node& root,
                                           std::string& error) {
    SimpleShooterState loaded_simple_shooter_state{};

    if (!SimpleShooterSerializer::load_from_node(
            root, loaded_simple_shooter_state, error)) {
      return false;
    }

    m_simple_shooter_state = loaded_simple_shooter_state;
    return true;
  }

  void
  SimpleShooterScene::apply_scene_settings_snapshot(const YAML::Node& node) {
    std::string error;
    if (!SimpleShooterSerializer::load_settings_from_node(
            node, m_simple_shooter_state.settings, error)) {
      return;
    }
  }

  bool SimpleShooterScene::restore_loaded_scene(SceneState& state,
                                                std::string&) {
    restore_loaded_shooter_concepts(state, m_simple_shooter_state);
    m_simple_shooter.reset(state, m_simple_shooter_state);
    return true;
  }

  GameObjectId SimpleShooterScene::create_wall_at_view_center() {
    GameObject wall = simple_shooter_factory::create_wall();

    wall.transform = first_free_view_center_spawn_position(wall);

    const GameObjectId id = m_state.objects.create_object(wall);

    if (GameObject* created = m_state.objects.get_by_id(id)) {
      created->identity.name = "Wall " + std::to_string(id);
    }

    m_state.objects.select(id);

    return id;
  }

  void SimpleShooterScene::draw_player_facing_indicator(
      SDL_Renderer* renderer) const {
    const GameObject* player = player_object();

    if (!player || !player->lifecycle.active || !player->render.visible) {
      return;
    }

    const Camera& camera = m_camera.active();

    const float player_center_x =
        player->transform.x + (static_cast<float>(player->size.width) * 0.5f);

    const float player_center_y =
        player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

    float indicator_x = player_center_x;
    float indicator_y = player_center_y;

    constexpr float indicator_distance = 10.0f;

    switch (player->motion.facing) {
    case Direction::Left:
      indicator_x -= indicator_distance;
      break;

    case Direction::Right:
      indicator_x += indicator_distance;
      break;

    case Direction::Up:
      indicator_y -= indicator_distance;
      break;

    case Direction::Down:
      indicator_y += indicator_distance;
      break;
    }

    const int screen_x =
        static_cast<int>((indicator_x - camera.x) * camera.zoom);
    const int screen_y =
        static_cast<int>((indicator_y - camera.y) * camera.zoom);

    const int size = std::max(2, static_cast<int>(3.0f * camera.zoom));

    SDL_Point points[4]{};

    switch (player->motion.facing) {
    case Direction::Left:
      points[0] = {screen_x - size, screen_y};
      points[1] = {screen_x + size, screen_y - size};
      points[2] = {screen_x + size, screen_y + size};
      points[3] = points[0];
      break;

    case Direction::Right:
      points[0] = {screen_x + size, screen_y};
      points[1] = {screen_x - size, screen_y - size};
      points[2] = {screen_x - size, screen_y + size};
      points[3] = points[0];
      break;

    case Direction::Up:
      points[0] = {screen_x, screen_y - size};
      points[1] = {screen_x - size, screen_y + size};
      points[2] = {screen_x + size, screen_y + size};
      points[3] = points[0];
      break;

    case Direction::Down:
      points[0] = {screen_x, screen_y + size};
      points[1] = {screen_x - size, screen_y - size};
      points[2] = {screen_x + size, screen_y - size};
      points[3] = points[0];
      break;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLines(renderer, points, 4);
  }

} // namespace prune
