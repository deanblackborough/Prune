#include <fstream>
#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "prune/resources/sprites.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/scene/simple_shooter_factory.hpp"

namespace prune {

    namespace {

        constexpr const char* k_behaviour_player = "simple_shooter.player";
        constexpr const char* k_behaviour_enemy = "simple_shooter.enemy";

        struct LoadedSceneState {
            GameObjectManager objects;
            GameObjectId player_id = k_invalid_game_object_id;
            GameObjectId selected_id = k_invalid_game_object_id;
            GridOptions grid_options{};
            SceneOptions scene_options{};
            CameraState camera_state{};
            float player_speed = 256.0f;
        };

        const char* to_string(GameObjectType type) noexcept
        {
            switch (type) {
            case GameObjectType::Runtime:
                return "runtime";
            case GameObjectType::Object:
            default:
                return "object";
            }
        }

        bool parse_game_object_type(const YAML::Node& node, GameObjectType& out)
        {
            if (!node) {
                return false;
            }

            const std::string value = node.as<std::string>();

            if (value == "object") {
                out = GameObjectType::Object;
                return true;
            }

            if (value == "runtime") {
                out = GameObjectType::Runtime;
                return true;
            }

            return false;
        }

        const char* to_string(RenderType type) noexcept
        {
            switch (type) {
            case RenderType::Sprite:
                return "sprite";
            case RenderType::Rectangle:
            default:
                return "rectangle";
            }
        }

        bool parse_render_type(const YAML::Node& node, RenderType& out)
        {
            if (!node) {
                return false;
            }

            const std::string value = node.as<std::string>();

            if (value == "rectangle") {
                out = RenderType::Rectangle;
                return true;
            }

            if (value == "sprite") {
                out = RenderType::Sprite;
                return true;
            }

            return false;
        }

        bool read_required_bool(const YAML::Node& node, const char* key, bool& out)
        {
            if (!node[key]) {
                return false;
            }

            out = node[key].as<bool>();
            return true;
        }

        bool read_required_int(const YAML::Node& node, const char* key, int& out)
        {
            if (!node[key]) {
                return false;
            }

            out = node[key].as<int>();
            return true;
        }

        bool read_required_uint(const YAML::Node& node, const char* key, GameObjectId& out)
        {
            if (!node[key]) {
                return false;
            }

            out = node[key].as<GameObjectId>();
            return true;
        }

        bool read_required_float(const YAML::Node& node, const char* key, float& out)
        {
            if (!node[key]) {
                return false;
            }

            out = node[key].as<float>();
            return true;
        }

        YAML::Node make_object_node(const GameObject& object)
        {
            YAML::Node node;
            node["id"] = object.id;
            node["name"] = object.name;
            node["type"] = to_string(object.type);
            node["active"] = object.active;

            node["transform"]["x"] = object.transform.x;
            node["transform"]["y"] = object.transform.y;

            node["size"]["width"] = object.size.width;
            node["size"]["height"] = object.size.height;

            node["collision"]["solid"] = object.collision.solid;

            node["render"]["type"] = to_string(object.render.type);
            node["render"]["visible"] = object.render.visible;

            switch (object.render.type) {
            case RenderType::Rectangle: {
                YAML::Node colour;
                colour.push_back(object.render.rectangle.color[0]);
                colour.push_back(object.render.rectangle.color[1]);
                colour.push_back(object.render.rectangle.color[2]);
                node["render"]["rectangle"]["colour"] = colour;
                break;
            }
            case RenderType::Sprite:
                node["render"]["sprite"]["sprite_key"] = object.render.sprite.sprite_key;
                break;
            }

            node["editor"]["selectable"] = object.editor.selectable;
            node["editor"]["renameable"] = object.editor.renameable;
            node["editor"]["movable"] = object.editor.movable;
            node["editor"]["deletable"] = object.editor.deletable;
            node["editor"]["cloneable"] = object.editor.cloneable;

            node["runtime"]["persistent"] = object.runtime.persistent;
            node["runtime"]["behaviour"] = object.runtime.behaviour;

            return node;
        }

        bool load_object_size_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            const YAML::Node size = node["size"];
            if (!size || !size.IsMap()) {
                error = "Object is missing size.";
                return false;
            }

            if (!read_required_int(size, "width", object.size.width) ||
                !read_required_int(size, "height", object.size.height)) {
                error = "Object size is incomplete.";
                return false;
            }

            return true;
        }

        bool load_object_collision_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            const YAML::Node collision = node["collision"];
            if (!collision || !collision.IsMap()) {
                error = "Object is missing collision.";
                return false;
            }

            if (!read_required_bool(collision, "solid", object.collision.solid)) {
                error = "Object collision is incomplete.";
                return false;
            }

            return true;
        }

        bool load_object_render_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            const YAML::Node render = node["render"];
            if (!render || !render.IsMap()) {
                error = "Object is missing render.";
                return false;
            }

            if (!parse_render_type(render["type"], object.render.type)) {
                error = "Object render.type is invalid.";
                return false;
            }

            if (!read_required_bool(render, "visible", object.render.visible)) {
                error = "Object render.visible is missing.";
                return false;
            }

            switch (object.render.type) {
            case RenderType::Rectangle: {
                const YAML::Node rectangle = render["rectangle"];
                const YAML::Node colour = rectangle ? rectangle["colour"] : YAML::Node{};

                if (!colour || !colour.IsSequence() || colour.size() != 3) {
                    error = "Object rectangle colour must be a 3-item sequence.";
                    return false;
                }

                object.render.rectangle.color[0] = colour[0].as<float>();
                object.render.rectangle.color[1] = colour[1].as<float>();
                object.render.rectangle.color[2] = colour[2].as<float>();
                break;
            }

            case RenderType::Sprite: {
                const YAML::Node sprite = render["sprite"];
                if (!sprite || !sprite.IsMap()) {
                    error = "Object sprite render data is missing.";
                    return false;
                }

                if (!sprite["sprite_key"]) {
                    error = "Object sprite.sprite_key is missing.";
                    return false;
                }

                const std::string sprite_key = sprite["sprite_key"].as<std::string>();

                if (find_sprite_resource(sprite_key) == nullptr) {
                    error = "Object sprite.sprite_key is not defined: " + sprite_key;
                    return false;
                }

                object.render.sprite.sprite_key = sprite_key;
                break;
            }
            }

            return true;
        }

        bool load_object_editor_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            const YAML::Node editor = node["editor"];
            if (!editor || !editor.IsMap()) {
                error = "Object is missing editor.";
                return false;
            }

            if (!read_required_bool(editor, "selectable", object.editor.selectable) ||
                !read_required_bool(editor, "renameable", object.editor.renameable) ||
                !read_required_bool(editor, "movable", object.editor.movable) ||
                !read_required_bool(editor, "deletable", object.editor.deletable) ||
                !read_required_bool(editor, "cloneable", object.editor.cloneable)) {
                error = "Object editor flags are incomplete.";
                return false;
            }

            return true;
        }

        bool load_object_runtime_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            const YAML::Node runtime = node["runtime"];
            if (!runtime || !runtime.IsMap()) {
                error = "Object is missing runtime.";
                return false;
            }

            if (!read_required_bool(runtime, "persistent", object.runtime.persistent)) {
                error = "Object runtime.persistent is missing.";
                return false;
            }

            if (!runtime["behaviour"]) {
                error = "Object runtime.behaviour is missing.";
                return false;
            }

            object.runtime.behaviour = runtime["behaviour"].as<std::string>();
            return true;
        }

        bool load_object_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            if (!node.IsMap()) {
                error = "Object entry is not a map.";
                return false;
            }

            if (!read_required_uint(node, "id", object.id)) {
                error = "Object is missing id.";
                return false;
            }

            if (!node["name"]) {
                error = "Object is missing name.";
                return false;
            }

            object.name = node["name"].as<std::string>();

            if (!parse_game_object_type(node["type"], object.type)) {
                error = "Object type is invalid.";
                return false;
            }

            if (!read_required_bool(node, "active", object.active)) {
                error = "Object active flag is missing.";
                return false;
            }

            const YAML::Node transform = node["transform"];
            if (!transform || !transform.IsMap()) {
                error = "Object is missing transform.";
                return false;
            }

            if (!read_required_float(transform, "x", object.transform.x) ||
                !read_required_float(transform, "y", object.transform.y)) {
                error = "Object transform is incomplete.";
                return false;
            }

            if (!load_object_size_from_node(node, object, error)) {
                return false;
            }

            if (!load_object_collision_from_node(node, object, error)) {
                return false;
            }

            if (!load_object_render_from_node(node, object, error)) {
                return false;
            }

            if (!load_object_editor_from_node(node, object, error)) {
                return false;
            }

            if (!load_object_runtime_from_node(node, object, error)) {
                return false;
            }

            object.velocity = {};
            object.lifetime = 0.0f;

            return true;
        }

        bool parse_camera_mode(const YAML::Node& node, CameraMode& out)
        {
            if (!node) {
                return false;
            }

            const std::string value = node.as<std::string>();

            if (value == "editor") {
                out = CameraMode::Editor;
                return true;
            }

            if (value == "game") {
                out = CameraMode::Game;
                return true;
            }

            return false;
        }
    }

    bool SceneSerializer::save_to_file(const SceneState& state, std::string_view path, std::string& error)
    {
        try {
            YAML::Node root;

            root["scene"]["next_object_id"] = state.objects.next_id();
            root["scene"]["player_id"] = state.player_id;

            if (state.objects.selected_id() != k_invalid_game_object_id) {
                root["scene"]["selected_object_id"] = state.objects.selected_id();
            }

            root["cameras"]["mode"] = state.camera.state().mode == CameraMode::Editor ? "editor" : "game";

            root["cameras"]["editor"]["x"] = state.camera.state().editor.x;
            root["cameras"]["editor"]["y"] = state.camera.state().editor.y;
            root["cameras"]["editor"]["speed"] = state.camera.state().editor.speed;
            root["cameras"]["editor"]["zoom"] = state.camera.state().editor.zoom;

            root["cameras"]["game"]["x"] = state.camera.state().game.x;
            root["cameras"]["game"]["y"] = state.camera.state().game.y;
            root["cameras"]["game"]["speed"] = state.camera.state().game.speed;
            root["cameras"]["game"]["zoom"] = state.camera.state().game.zoom;
            root["cameras"]["game"]["follow_player"] = state.camera.state().game_options.follow_player;

            root["grid"]["show_grid"] = state.grid_options.show_grid;
            root["grid"]["snap_to_grid"] = state.grid_options.snap_to_grid;
            root["grid"]["grid_size"] = state.grid_options.grid_size;
            root["grid"]["nudge_step"] = state.grid_options.nudge_step;
            root["grid"]["shift_nudge_steps"] = state.grid_options.shift_nudge_steps;

            root["options"]["highlight_selected"] = state.scene_options.highlight_selected;

            root["player"]["speed"] = state.player_controller.speed();

            YAML::Node objects = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& object : state.objects.objects()) {
                if (!object.runtime.persistent) {
                    continue;
                }

                objects.push_back(make_object_node(object));
            }

            root["objects"] = objects;

            std::ofstream output{ std::string(path) };
            if (!output.is_open()) {
                error = "Could not open save file for writing.";
                return false;
            }

            output << root;
            return true;
        }
        catch (const YAML::Exception& ex) {
            error = ex.what();
            return false;
        }
        catch (const std::exception& ex) {
            error = ex.what();
            return false;
        }
    }

    bool SceneSerializer::load_from_file(SceneState& state, std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            const YAML::Node scene = root["scene"];
            const YAML::Node cameras = root["cameras"];
            const YAML::Node player_node = root["player"];
            const YAML::Node grid = root["grid"];
            const YAML::Node options = root["options"];
            const YAML::Node objects = root["objects"];

            if (!scene || !cameras || !player_node || !grid || !options || !objects || !objects.IsSequence()) {
                error = "Save file is missing required top-level sections.";
                return false;
            }

            LoadedSceneState loaded{};

            if (!read_required_float(player_node, "speed", loaded.player_speed)) {
                error = "player.speed is missing.";
                return false;
            }

            GameObjectId loaded_next_id = 1;
            GameObjectId loaded_player_id = k_invalid_game_object_id;
            GameObjectId loaded_selected_id = k_invalid_game_object_id;

            if (!read_required_uint(scene, "next_object_id", loaded_next_id)) {
                error = "scene.next_object_id is missing.";
                return false;
            }

            if (!read_required_uint(scene, "player_id", loaded_player_id)) {
                error = "scene.player_id is missing.";
                return false;
            }

            if (scene["selected_object_id"]) {
                loaded_selected_id = scene["selected_object_id"].as<GameObjectId>();
            }

            const YAML::Node editor = cameras["editor"];
            const YAML::Node game = cameras["game"];

            if (!editor || !game) {
                error = "cameras section is incomplete.";
                return false;
            }

            if (!parse_camera_mode(cameras["mode"], loaded.camera_state.mode)) {
                error = "cameras.mode is invalid.";
                return false;
            }

            if (!read_required_float(editor, "x", loaded.camera_state.editor.x) ||
                !read_required_float(editor, "y", loaded.camera_state.editor.y) ||
                !read_required_float(editor, "speed", loaded.camera_state.editor.speed) ||
                !read_required_float(editor, "zoom", loaded.camera_state.editor.zoom)) {
                error = "cameras.editor is incomplete.";
                return false;
            }

            if (!read_required_float(game, "x", loaded.camera_state.game.x) ||
                !read_required_float(game, "y", loaded.camera_state.game.y) ||
                !read_required_float(game, "speed", loaded.camera_state.game.speed) ||
                !read_required_float(game, "zoom", loaded.camera_state.game.zoom) ||
                !read_required_bool(game, "follow_player", loaded.camera_state.game_options.follow_player)) {
                error = "cameras.game is incomplete.";
                return false;
            }

            if (!read_required_bool(grid, "show_grid", loaded.grid_options.show_grid) ||
                !read_required_bool(grid, "snap_to_grid", loaded.grid_options.snap_to_grid) ||
                !read_required_int(grid, "grid_size", loaded.grid_options.grid_size) ||
                !read_required_int(grid, "nudge_step", loaded.grid_options.nudge_step) ||
                !read_required_int(grid, "shift_nudge_steps", loaded.grid_options.shift_nudge_steps)) {
                error = "grid is incomplete.";
                return false;
            }

            if (!read_required_bool(options, "highlight_selected", loaded.scene_options.highlight_selected)) {
                error = "options.highlight_selected is missing.";
                return false;
            }

            int player_count = 0;

            for (const auto& entry : objects) {
                GameObject object{};

                if (!load_object_from_node(entry, object, error)) {
                    return false;
                }

                if (!object.runtime.persistent) {
                    error = "Save file contains a non-persistent object.";
                    return false;
                }

                if (!loaded.objects.add_loaded_object(object)) {
                    error = "Failed to restore object. Duplicate or invalid id.";
                    return false;
                }

                if (object.runtime.behaviour == k_behaviour_player) {
                    ++player_count;
                }
            }

            if (loaded.objects.empty()) {
                error = "Save file contains no objects.";
                return false;
            }

            if (player_count != 1) {
                error = "Save file must contain exactly one player object.";
                return false;
            }

            loaded.player_id = loaded_player_id;

            const GameObject* loaded_player = loaded.objects.get_by_id(loaded.player_id);
            if (!loaded_player) {
                error = "Saved player_id does not exist.";
                return false;
            }

            if (loaded_player->runtime.behaviour != k_behaviour_player) {
                error = "Saved player_id does not point to a player object.";
                return false;
            }

            GameObjectId max_loaded_id = k_invalid_game_object_id;
            for (const auto& object : loaded.objects.objects()) {
                if (object.id > max_loaded_id) {
                    max_loaded_id = object.id;
                }
            }

            if (loaded_next_id <= max_loaded_id) {
                loaded_next_id = max_loaded_id + 1;
            }

            loaded.objects.set_next_id(loaded_next_id);

            if (loaded_selected_id != k_invalid_game_object_id &&
                loaded.objects.get_by_id(loaded_selected_id) != nullptr) {
                loaded.selected_id = loaded_selected_id;
            }
            else {
                loaded.selected_id = loaded.player_id;
            }

            loaded.objects.set_selected_id(loaded.selected_id);

            state.objects = std::move(loaded.objects);
            state.player_id = loaded.player_id;
            state.enemy_id = k_invalid_game_object_id;

            for (const auto& object : state.objects.objects()) {
                if (object.runtime.behaviour == k_behaviour_enemy) {
                    state.enemy_id = object.id;
                    break;
                }
            }

            if (state.enemy_id == k_invalid_game_object_id) {
                state.enemy_id = state.objects.create_object(simple_shooter_factory::create_enemy());
            }

            state.grid_options = loaded.grid_options;
            state.scene_options = loaded.scene_options;
            state.camera.state() = loaded.camera_state;
            state.player_controller.set_speed(loaded.player_speed);

            state.camera.update_game_camera(state.viewport, state.objects.get_by_id(state.player_id));

            return true;
        }
        catch (const YAML::Exception& ex) {
            error = ex.what();
            return false;
        }
        catch (const std::exception& ex) {
            error = ex.what();
            return false;
        }
    }
}
