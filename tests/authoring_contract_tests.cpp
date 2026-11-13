#include <cstdio>
#include <string>
#include <string_view>

#include <yaml-cpp/yaml.h>

#include <catch2/catch_test_macros.hpp>

#include "prune/editor/grid_options.hpp"
#include "prune/scene/world_scene.hpp"

namespace prune {

  namespace {

    // A minimal, ImGui-free WorldScene subclass used only to exercise the
    // authoring contract (commands, undo/redo, dirty, save/load) without any
    // scene-specific gameplay behaviour. It owns one tiny "setting" so the
    // generic SceneSettingsChange path can be driven directly, bypassing the
    // ImGui widgets that normally arm it.
    class AuthoringTestScene final : public WorldScene {
    public:
      explicit AuthoringTestScene(GridOptions& grid_options) noexcept
          : WorldScene(grid_options) {}

      void on_exit() override {}
      void new_scene() override {}

      [[nodiscard]] std::string_view
      default_file_path() const noexcept override {
        return m_file_path;
      }
      [[nodiscard]] std::string_view scene_type_id() const noexcept override {
        return "authoring_test";
      }
      [[nodiscard]] std::string_view scene_name() const noexcept override {
        return "Authoring Test";
      }
      [[nodiscard]] std::string_view
      scene_tools_label() const noexcept override {
        return "Authoring Test";
      }
      [[nodiscard]] float setting_value() const noexcept {
        return m_setting_value;
      }
      void set_setting_value_live(float value) noexcept {
        m_setting_value = value;
      }

      [[nodiscard]] static YAML::Node settings_snapshot(float value) {
        YAML::Node node;
        node["value"] = value;
        return node;
      }

    protected:
      void update_runtime(WorldState&, float, const Input&, bool) override {}
      void restart_runtime() override {}

      void save_scene_data(YAML::Node& root) const override {
        root["authoring_test"]["value"] = m_setting_value;
      }

      [[nodiscard]] bool load_scene_data(const YAML::Node& root,
                                         std::string&) override {
        const YAML::Node section = root["authoring_test"];
        if (section && section["value"]) {
          m_setting_value = section["value"].as<float>();
        }
        return true;
      }

      [[nodiscard]] bool restore_loaded_scene(SceneState&,
                                              std::string&) override {
        return true;
      }

      void apply_scene_settings_snapshot(const YAML::Node& node) override {
        if (node["value"]) {
          m_setting_value = node["value"].as<float>();
        }
      }

    private:
      float m_setting_value = 0.0f;
      std::string m_file_path = "authoring_test_scene.yaml";
    };

    [[nodiscard]] GameObject make_test_object(std::string_view name, float x,
                                              float y) {
      GameObject object{};
      object.identity.name = std::string(name);
      object.transform.x = x;
      object.transform.y = y;
      object.size.width = 16;
      object.size.height = 16;
      return object;
    }

    [[nodiscard]] GameObjectId create_authored_object(AuthoringTestScene& scene,
                                                      std::string_view name,
                                                      float x, float y) {
      const GameObject object = make_test_object(name, x, y);
      const GameObjectId id = scene.get_object_manager().create_object(object);
      const GameObject* created = scene.get_object_manager().get_by_id(id);
      REQUIRE(created != nullptr);
      scene.record_editor_command(make_object_create(*created, "create"));
      return id;
    }

    struct TempFile {
      std::string path;

      explicit TempFile(std::string_view name)
          : path(std::string("authoring_contract_test_") + std::string(name) +
                 ".yaml") {}

      ~TempFile() { std::remove(path.c_str()); }
    };

  } // namespace

  TEST_CASE("edit then runtime mutation then undo restores the authored value",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);

    GameObject* live = scene.get_object_manager().get_by_id(id);
    REQUIRE(live != nullptr);

    const GameObject before = *live;
    live->transform.x = 50.0f;
    const GameObject after = *live;
    scene.record_editor_command(make_object_change("move", before, after));

    // Simulate runtime drift: the object moves during play without going
    // through a command.
    live->transform.x = 999.0f;

    REQUIRE(scene.undo_editor_command());

    const GameObject* restored = scene.get_object_manager().get_by_id(id);
    REQUIRE(restored != nullptr);
    CHECK(restored->transform.x == before.transform.x);
  }

  TEST_CASE("edit then runtime mutation then reset produces the authored value",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);

    GameObject* live = scene.get_object_manager().get_by_id(id);
    REQUIRE(live != nullptr);

    const GameObject before = *live;
    live->transform.x = 50.0f;
    const GameObject after = *live;
    scene.record_editor_command(make_object_change("move", before, after));

    live->transform.x = 999.0f;

    scene.reset_runtime();

    const GameObject* reset = scene.get_object_manager().get_by_id(id);
    REQUIRE(reset != nullptr);
    CHECK(reset->transform.x == after.transform.x);
  }

  TEST_CASE("edit, save, mutate, undo, save round-trips correctly",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);

    GameObject* live = scene.get_object_manager().get_by_id(id);
    REQUIRE(live != nullptr);

    const GameObject before = *live;
    live->transform.x = 50.0f;
    const GameObject after = *live;
    scene.record_editor_command(make_object_change("move", before, after));

    TempFile first_save("round_trip_1");
    std::string error;
    REQUIRE(scene.save_to_file(first_save.path, error));

    live = scene.get_object_manager().get_by_id(id);
    REQUIRE(live != nullptr);
    live->transform.x = 999.0f;

    REQUIRE(scene.undo_editor_command());

    TempFile second_save("round_trip_2");
    REQUIRE(scene.save_to_file(second_save.path, error));

    AuthoringTestScene loaded(grid_options);
    REQUIRE(loaded.load_from_file(second_save.path, error));

    const GameObject* loaded_object = loaded.get_object_manager().get_by_id(id);
    REQUIRE(loaded_object != nullptr);
    CHECK(loaded_object->transform.x == before.transform.x);
  }

  TEST_CASE("create, undo, redo, reset", "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);
    CHECK(scene.get_object_manager().get_by_id(id) != nullptr);

    REQUIRE(scene.undo_editor_command());
    CHECK(scene.get_object_manager().get_by_id(id) == nullptr);

    REQUIRE(scene.redo_editor_command());
    CHECK(scene.get_object_manager().get_by_id(id) != nullptr);

    scene.reset_runtime();
    CHECK(scene.get_object_manager().get_by_id(id) != nullptr);
  }

  TEST_CASE("delete, runtime continues, undo restores the object",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);
    const GameObject* existing = scene.get_object_manager().get_by_id(id);
    REQUIRE(existing != nullptr);
    const GameObject deleted_copy = *existing;

    scene.get_object_manager().remove_object(id);
    scene.record_editor_command(make_object_delete(deleted_copy, "delete"));
    CHECK(scene.get_object_manager().get_by_id(id) == nullptr);

    // "Runtime continues" — a few update ticks with nothing to undo yet.
    Input input;
    for (int i = 0; i < 3; ++i) {
      scene.update(1.0f / 60.0f, input);
    }

    REQUIRE(scene.undo_editor_command());
    const GameObject* restored = scene.get_object_manager().get_by_id(id);
    REQUIRE(restored != nullptr);
    CHECK(restored->transform.x == deleted_copy.transform.x);
  }

  TEST_CASE("multi-move then reset", "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id_a = create_authored_object(scene, "A", 0.0f, 0.0f);
    const GameObjectId id_b = create_authored_object(scene, "B", 100.0f, 0.0f);

    GameObject* a = scene.get_object_manager().get_by_id(id_a);
    GameObject* b = scene.get_object_manager().get_by_id(id_b);
    REQUIRE(a != nullptr);
    REQUIRE(b != nullptr);

    const std::vector<GameObject> before = {*a, *b};
    a->transform.x += 10.0f;
    b->transform.x += 10.0f;
    const std::vector<GameObject> after = {*a, *b};

    scene.record_editor_command(
        make_objects_change("multi-move", before, after));

    scene.reset_runtime();

    const GameObject* reset_a = scene.get_object_manager().get_by_id(id_a);
    const GameObject* reset_b = scene.get_object_manager().get_by_id(id_b);
    REQUIRE(reset_a != nullptr);
    REQUIRE(reset_b != nullptr);
    CHECK(reset_a->transform.x == 10.0f);
    CHECK(reset_b->transform.x == 110.0f);
  }

  TEST_CASE("z-index change, undo, save, load", "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    const GameObjectId id = create_authored_object(scene, "Box", 10.0f, 20.0f);

    GameObject* live = scene.get_object_manager().get_by_id(id);
    REQUIRE(live != nullptr);

    const GameObject before = *live;
    live->render.z_index = 7;
    const GameObject after = *live;
    scene.record_editor_command(make_object_change("z-index", before, after));

    REQUIRE(scene.undo_editor_command());

    TempFile save_file("z_index");
    std::string error;
    REQUIRE(scene.save_to_file(save_file.path, error));

    AuthoringTestScene loaded(grid_options);
    REQUIRE(loaded.load_from_file(save_file.path, error));

    const GameObject* loaded_object = loaded.get_object_manager().get_by_id(id);
    REQUIRE(loaded_object != nullptr);
    CHECK(loaded_object->render.z_index == before.render.z_index);
  }

  TEST_CASE("scene setting change marks dirty, undo, redo, save, load",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    // The generic scene serializer requires at least one persistable object
    // in the save file; this scenario is about the settings path, not
    // objects, so one throwaway object (saved once, up front) is enough to
    // satisfy that without the object creation itself affecting the dirty
    // checks below.
    [[maybe_unused]] const GameObjectId setup_id =
        create_authored_object(scene, "Box", 0.0f, 0.0f);
    std::string setup_error;
    TempFile setup_save("scene_setting_setup");
    REQUIRE(scene.save_to_file(setup_save.path, setup_error));
    CHECK_FALSE(scene.is_dirty());

    const YAML::Node before_snapshot =
        AuthoringTestScene::settings_snapshot(scene.setting_value());

    scene.set_setting_value_live(42.0f);

    const YAML::Node after_snapshot =
        AuthoringTestScene::settings_snapshot(scene.setting_value());

    scene.record_editor_command(make_scene_settings_change(
        "Change value", before_snapshot, after_snapshot));

    CHECK(scene.is_dirty());
    CHECK(scene.setting_value() == 42.0f);

    REQUIRE(scene.undo_editor_command());
    CHECK(scene.setting_value() == 0.0f);

    REQUIRE(scene.redo_editor_command());
    CHECK(scene.setting_value() == 42.0f);

    TempFile save_file("scene_setting");
    std::string error;
    REQUIRE(scene.save_to_file(save_file.path, error));

    AuthoringTestScene loaded(grid_options);
    REQUIRE(loaded.load_from_file(save_file.path, error));
    CHECK(loaded.setting_value() == 42.0f);
  }

  TEST_CASE("pause then save then load does not persist or restore a paused "
            "runtime",
            "[authoring]") {
    GridOptions grid_options{};
    AuthoringTestScene scene(grid_options);
    scene.new_scene();
    scene.on_enter();

    [[maybe_unused]] const GameObjectId id =
        create_authored_object(scene, "Box", 10.0f, 20.0f);

    scene.play_runtime();
    CHECK_FALSE(scene.runtime_paused());

    scene.pause_runtime();
    CHECK(scene.runtime_paused());

    TempFile save_file("pause");
    std::string error;
    REQUIRE(scene.save_to_file(save_file.path, error));

    AuthoringTestScene loaded(grid_options);
    REQUIRE(loaded.load_from_file(save_file.path, error));

    // A freshly loaded scene starts stopped (paused), but that comes from
    // the normal load lifecycle, not from the paused state being persisted.
    // Playing it should behave like a fresh scene, not stay paused.
    loaded.play_runtime();
    CHECK_FALSE(loaded.runtime_paused());
  }

} // namespace prune
