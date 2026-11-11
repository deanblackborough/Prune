#include <array>
#include <string>

#include <yaml-cpp/yaml.h>

#include "prune/scene/platformer/platformer_concepts.hpp"
#include "prune/scene/platformer/platformer_factory.hpp"
#include "prune/scene/platformer/platformer_scene.hpp"
#include "prune/scene/platformer/platformer_serializer.hpp"
#include "prune/scene/scene_factory.hpp"

namespace prune {

  namespace {

    [[nodiscard]] GameObjectId
    first_object_id_for_kind(const SceneState& state,
                             platformer_concepts::ObjectKind kind) noexcept {
      for (const auto& object : state.world.objects.objects()) {
        if (platformer_concepts::kind_for(object) == kind) {
          return object.identity.id;
        }
      }

      return k_invalid_game_object_id;
    }

    [[nodiscard]] bool
    object_has_kind(const SceneState& state, GameObjectId id,
                    platformer_concepts::ObjectKind kind) noexcept {
      const GameObject* object = state.world.objects.get_by_id(id);
      return object && platformer_concepts::kind_for(*object) == kind;
    }

    void restore_loaded_platformer_concepts(SceneState& state,
                                            PlatformerState& platformer_state) {
      if (!object_has_kind(state, platformer_state.player_id,
                           platformer_concepts::ObjectKind::Player)) {
        platformer_state.player_id = first_object_id_for_kind(
            state, platformer_concepts::ObjectKind::Player);
      }

      if (GameObject* player =
              state.world.objects.get_by_id(platformer_state.player_id)) {
        player->editor.movable = true;
      }

      if (!object_has_kind(state, platformer_state.player_start_id,
                           platformer_concepts::ObjectKind::PlayerStart)) {
        platformer_state.player_start_id = first_object_id_for_kind(
            state, platformer_concepts::ObjectKind::PlayerStart);
      }

      if (platformer_state.player_start_id == k_invalid_game_object_id) {
        platformer_state.player_start_id = state.world.objects.create_object(
            platformer_factory::create_player_start(32.0f, 112.0f));
      }
    }
  } // namespace

  PlatformerScene::PlatformerScene(int window_width, int window_height,
                                   GridOptions& grid_options)
      : WorldScene(grid_options) {
    m_state.editor.viewport.width = window_width;
    m_state.editor.viewport.height = window_height;
  }

  void PlatformerScene::on_exit() {
    m_state.world.objects.clear();
    m_platformer_state.player_id = k_invalid_game_object_id;
    m_platformer_state.player_start_id = k_invalid_game_object_id;
  }

  void PlatformerScene::update_runtime(WorldState& world, float dt,
                                       const Input& input,
                                       bool keyboard_input_enabled) {
    m_platformer.update(world, m_camera, m_platformer_state, dt, input,
                        keyboard_input_enabled);
  }

  void PlatformerScene::restart_runtime() { m_platformer_state.runtime = {}; }

  void PlatformerScene::set_runtime_paused(bool paused) noexcept {
    m_platformer_state.runtime.paused = paused;
  }

  bool PlatformerScene::is_runtime_paused() const noexcept {
    return m_platformer_state.runtime.paused;
  }

  GameObject* PlatformerScene::game_camera_target() noexcept {
    return player_object();
  }

  void PlatformerScene::reset_runtime_state() {
    m_state.world.objects.clear();
    m_platformer_state = {};
    m_camera.reset();
    m_state.editor.scene_options = {};
    m_state.editor.drag_state = {};
    m_state.editor.editor_tool = EditorTool::Select;
    m_state.editor.editor_commands.clear();
  }

  void PlatformerScene::restore_defaults() {
    reset_runtime_state();

    m_platformer_state.player_start_id = m_state.world.objects.create_object(
        platformer_factory::create_player_start(32.0f, 112.0f));

    m_platformer_state.player_id = m_state.world.objects.create_object(
        platformer_factory::create_player());

    m_state.world.objects.create_object(platformer_factory::create_platform(
        0.0f, 176.0f, 400, 16, "Main Platform"));
    m_state.world.objects.create_object(platformer_factory::create_platform(
        96.0f, 144.0f, 80, 16, "Lower Step"));
    m_state.world.objects.create_object(platformer_factory::create_platform(
        208.0f, 112.0f, 80, 16, "Mid Platform"));
    m_state.world.objects.create_object(platformer_factory::create_platform(
        320.0f, 80.0f, 64, 16, "High Platform"));
    m_state.world.objects.create_object(
        platformer_factory::create_hazard(144.0f, 160.0f, 32, 16, "Spike Pit"));
    m_state.world.objects.create_object(platformer_factory::create_hazard(
        288.0f, 160.0f, 32, 16, "Second Spike Pit"));

    m_platformer.reset_player(m_state.world, m_platformer_state);
    m_state.editor.selection.select(m_state.world.objects,
                                    m_platformer_state.player_id);

    establish_game_camera();
  }

  void PlatformerScene::establish_game_camera() {
    m_camera.game().zoom = 3.0f;
    m_camera.editor().zoom = 3.0f;
    m_camera.update_game_camera(m_state.editor.viewport, player_object());
    m_camera.editor().x = m_camera.game().x;
    m_camera.editor().y = m_camera.game().y;
  }

  void PlatformerScene::new_scene() { restore_defaults(); }

  ObjectConcept
  PlatformerScene::object_concept_for(const GameObject& object) const {
    return platformer_concepts::describe_object(object);
  }

  std::span<const SceneCreationAction>
  PlatformerScene::scene_creation_actions() const noexcept {
    static constexpr std::array<SceneCreationAction, 2> actions{
        {{"platform", "Platform"}, {"hazard", "Hazard"}}};

    return actions;
  }

  GameObjectId
  PlatformerScene::create_scene_object(std::string_view action_id) {
    if (action_id == "platform") {
      return add_platform_at_view_center();
    }

    if (action_id == "hazard") {
      return add_hazard_at_view_center();
    }

    return k_invalid_game_object_id;
  }

  std::string_view PlatformerScene::default_file_path() const noexcept {
    if (const SceneDescriptor* descriptor =
            scene_descriptor_for(SceneType::Platformer)) {
      return descriptor->default_file_path;
    }

    return {};
  }

  std::string_view PlatformerScene::scene_type_id() const noexcept {
    if (const SceneDescriptor* descriptor =
            scene_descriptor_for(SceneType::Platformer)) {
      return descriptor->id;
    }

    return {};
  }

  void PlatformerScene::save_scene_data(YAML::Node& root) const {
    PlatformerSerializer::save_to_node(m_platformer_state, root);
  }

  bool PlatformerScene::load_scene_data(const YAML::Node& root,
                                        std::string& error) {
    PlatformerState loaded_platformer_state{};

    if (!PlatformerSerializer::load_from_node(root, loaded_platformer_state,
                                              error)) {
      return false;
    }

    m_platformer_state = loaded_platformer_state;
    return true;
  }

  void PlatformerScene::apply_scene_settings_snapshot(const YAML::Node& node) {
    PlatformerSerializer::load_settings_from_node(node,
                                                  m_platformer_state.settings);
  }

  bool PlatformerScene::restore_loaded_scene(SceneState& state,
                                             std::string& error) {
    restore_loaded_platformer_concepts(state, m_platformer_state);

    if (m_platformer_state.player_id == k_invalid_game_object_id) {
      error = "Platformer scene is missing its player object.";
      return false;
    }

    return true;
  }

  GameObject* PlatformerScene::player_object() noexcept {
    return m_state.world.objects.get_by_id(m_platformer_state.player_id);
  }

  const GameObject* PlatformerScene::player_object() const noexcept {
    return m_state.world.objects.get_by_id(m_platformer_state.player_id);
  }

  GameObjectId PlatformerScene::add_platform_at_view_center() {
    GameObject platform =
        platformer_factory::create_platform(0.0f, 0.0f, 64, 16, "Platform");
    platform.transform = first_free_view_center_spawn_position(platform);

    const GameObjectId id = m_state.world.objects.create_object(platform);
    if (GameObject* created = m_state.world.objects.get_by_id(id)) {
      created->identity.name = "Platform " + std::to_string(id);
    }

    m_state.editor.selection.select(m_state.world.objects, id);
    return id;
  }

  GameObjectId PlatformerScene::add_hazard_at_view_center() {
    GameObject hazard = platformer_factory::create_hazard(0.0f, 0.0f);
    hazard.transform = first_free_view_center_spawn_position(hazard);

    const GameObjectId id = m_state.world.objects.create_object(hazard);
    if (GameObject* created = m_state.world.objects.get_by_id(id)) {
      created->identity.name = "Hazard " + std::to_string(id);
    }

    m_state.editor.selection.select(m_state.world.objects, id);
    return id;
  }

} // namespace prune
