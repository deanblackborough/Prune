
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

#include <yaml-cpp/yaml.h>

#include "prune/scene/sandbox_scene.hpp"
#include "prune/resources/sprites.hpp"

namespace prune {

    namespace {

        struct LoadedSceneState {
            GameObjectManager objects;
            GameObjectId player_id = k_invalid_game_object_id;
            GameObjectId selected_id = k_invalid_game_object_id;
            GridOptions grid_options{};
            SceneOptions scene_options{};
            CameraState cameras{};
            float player_speed = 256.0f;
        };

        const char* to_string(GameObjectKind kind) noexcept
        {
            switch (kind) {
            case GameObjectKind::Player:
                return "player";
            case GameObjectKind::Block:
                return "block";
            case GameObjectKind::Enemy:
                return "enemy";
            case GameObjectKind::Bullet:
                return "bullet";
            case GameObjectKind::Generic:
            default:
                return "generic";
            }
        }

        bool parse_game_object_kind(const YAML::Node& node, GameObjectKind& out)
        {
            if (!node) {
                return false;
            }

            const std::string value = node.as<std::string>();

            if (value == "player") {
                out = GameObjectKind::Player;
                return true;
            }

            if (value == "block") {
                out = GameObjectKind::Block;
                return true;
            }

            if (value == "enemy") {
                out = GameObjectKind::Enemy;
                return true;
            }

            if (value == "bullet") {
                out = GameObjectKind::Bullet;
                return true;
            }

            if (value == "generic") {
                out = GameObjectKind::Generic;
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

        YAML::Node make_object_node(const GameObject& object)
        {
            YAML::Node node;
            node["id"] = object.id;
            node["name"] = object.name;
            node["kind"] = to_string(object.kind);
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
            case RenderType::Sprite: {
                node["render"]["sprite"]["sprite_key"] = object.render.sprite.sprite_key;
                break;
            }
            }

            return node;
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

        bool load_object_kind_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            if (node["kind"]) {
                if (!parse_game_object_kind(node["kind"], object.kind)) {
                    error = "Object kind is invalid.";
                    return false;
                }

                return true;
            }

            if (node["is_player"]) {
                const bool is_player = node["is_player"].as<bool>();
                object.kind = is_player ? GameObjectKind::Player : GameObjectKind::Block;
                return true;
            }

            error = "Object is missing kind.";
            return false;
        }

        bool load_object_size_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            if (const YAML::Node size = node["size"]; size && size.IsMap()) {
                if (!read_required_int(size, "width", object.size.width) ||
                    !read_required_int(size, "height", object.size.height)) {
                    error = "Object size is incomplete.";
                    return false;
                }

                return true;
            }

            if (const YAML::Node rectangle = node["rectangle"]; rectangle && rectangle.IsMap()) {
                if (!read_required_int(rectangle, "width", object.size.width) ||
                    !read_required_int(rectangle, "height", object.size.height)) {
                    error = "Object rectangle is incomplete.";
                    return false;
                }

                return true;
            }

            error = "Object is missing size.";
            return false;
        }

        bool load_object_collision_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            if (const YAML::Node collision = node["collision"]; collision && collision.IsMap()) {
                if (!read_required_bool(collision, "solid", object.collision.solid)) {
                    error = "Object collision is incomplete.";
                    return false;
                }

                return true;
            }

            if (!read_required_bool(node, "solid", object.collision.solid)) {
                error = "Object solid flag is missing.";
                return false;
            }

            return true;
        }

        bool load_object_render_from_node(const YAML::Node& node, GameObject& object, std::string& error)
        {
            if (const YAML::Node render = node["render"]; render && render.IsMap()) {
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

					//@todo: validate sprite key exists in assets when we have an asset library
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

            // Legacy format fallback
            object.render.type = RenderType::Rectangle;

            if (!read_required_bool(node, "visible", object.render.visible)) {
                error = "Object visible flag is missing.";
                return false;
            }

            const YAML::Node colour = node["colour"];
            if (!colour || !colour.IsSequence() || colour.size() != 3) {
                error = "Object colour must be a 3-item sequence.";
                return false;
            }

            object.render.rectangle.color[0] = colour[0].as<float>();
            object.render.rectangle.color[1] = colour[1].as<float>();
            object.render.rectangle.color[2] = colour[2].as<float>();

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

            if (!load_object_kind_from_node(node, object, error)) {
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

            object.velocity = {};
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

    bool SandboxScene::save_to_file(std::string_view path, std::string& error) const
    {
        try {
            YAML::Node root;

            root["scene"]["next_object_id"] = m_objects.next_id();
            root["scene"]["player_id"] = m_player_id;

            if (m_objects.selected_id() != k_invalid_game_object_id) {
                root["scene"]["selected_object_id"] = m_objects.selected_id();
            }

            root["cameras"]["mode"] = (m_cameras.mode == CameraMode::Editor) ? "editor" : "game";

            root["cameras"]["editor"]["x"] = m_cameras.editor.x;
            root["cameras"]["editor"]["y"] = m_cameras.editor.y;
            root["cameras"]["editor"]["speed"] = m_cameras.editor.speed;
            root["cameras"]["editor"]["zoom"] = m_cameras.editor.zoom;

            root["cameras"]["game"]["x"] = m_cameras.game.x;
            root["cameras"]["game"]["y"] = m_cameras.game.y;
            root["cameras"]["game"]["speed"] = m_cameras.game.speed;
			root["cameras"]["game"]["zoom"] = m_cameras.game.zoom;
            root["cameras"]["game"]["follow_player"] = m_cameras.game_options.follow_player;

            root["grid"]["show_grid"] = m_grid_options.show_grid;
            root["grid"]["snap_to_grid"] = m_grid_options.snap_to_grid;
            root["grid"]["grid_size"] = m_grid_options.grid_size;
            root["grid"]["nudge_step"] = m_grid_options.nudge_step;
            root["grid"]["shift_nudge_steps"] = m_grid_options.shift_nudge_steps;

            root["options"]["highlight_selected"] = m_scene_options.highlight_selected;

            root["player"]["speed"] = m_player_controller.speed();

            YAML::Node objects = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& object : m_objects.objects()) {
                if (object.kind == GameObjectKind::Bullet) {
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

    bool SandboxScene::load_from_file(std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            const YAML::Node scene = root["scene"];
            const YAML::Node cameras = root["cameras"];
            const YAML::Node player_node = root["player"];
            const YAML::Node legacy_camera = root["camera"];
            const YAML::Node grid = root["grid"];
            const YAML::Node options = root["options"];
            const YAML::Node objects = root["objects"];

            if (!scene || !grid || !options || !objects || !objects.IsSequence()) {
                error = "Save file is missing required top-level sections.";
                return false;
            }

            LoadedSceneState loaded{};

            if (player_node && player_node["speed"]) {
                loaded.player_speed = player_node["speed"].as<float>();
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

            if (cameras) {
                const YAML::Node editor = cameras["editor"];
                const YAML::Node game = cameras["game"];

                if (!editor || !game) {
                    error = "cameras section is incomplete.";
                    return false;
                }

                if (!parse_camera_mode(cameras["mode"], loaded.cameras.mode)) {
                    error = "cameras.mode is invalid.";
                    return false;
                }

                if (!read_required_float(editor, "x", loaded.cameras.editor.x) ||
                    !read_required_float(editor, "y", loaded.cameras.editor.y) ||
                    !read_required_float(editor, "speed", loaded.cameras.editor.speed) || 
                    !read_required_float(editor, "zoom", loaded.cameras.editor.zoom)) {
                    error = "cameras.editor is incomplete.";
                    return false;
                }

                if (!read_required_float(game, "x", loaded.cameras.game.x) ||
                    !read_required_float(game, "y", loaded.cameras.game.y) ||
                    !read_required_float(game, "speed", loaded.cameras.game.speed) ||
					!read_required_float(game, "zoom", loaded.cameras.game.zoom) ||
                    !read_required_bool(game, "follow_player", loaded.cameras.game_options.follow_player)) {
                    error = "cameras.game is incomplete.";
                    return false;
                }
            }
            else if (legacy_camera) {
                if (!read_required_float(legacy_camera, "x", loaded.cameras.editor.x) ||
                    !read_required_float(legacy_camera, "y", loaded.cameras.editor.y)) {
                    error = "legacy camera is incomplete.";
                    return false;
                }

                loaded.cameras.editor.speed = 256.0f;
				loaded.cameras.editor.zoom = 1.0f;
                loaded.cameras.game.speed = 96.0f;
				loaded.cameras.game.zoom = 3.0f;
                loaded.cameras.mode = CameraMode::Editor;
                loaded.cameras.game_options.follow_player = true;
            }
            else {
                error = "Save file is missing cameras section.";
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

                if (!loaded.objects.add_loaded_object(object)) {
                    error = "Failed to restore object. Duplicate or invalid id.";
                    return false;
                }

                if (object.kind == GameObjectKind::Player) {
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

            if (loaded_player->kind != GameObjectKind::Player) {
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

            m_objects = std::move(loaded.objects);
            m_player_id = loaded.player_id;
            m_enemy_id = k_invalid_game_object_id;

            for (const auto& object : m_objects.objects()) {
                if (object.kind == GameObjectKind::Enemy) {
                    m_enemy_id = object.id;
                    break;
                }
            }

            if (m_enemy_id == k_invalid_game_object_id) {
                m_enemy_id = m_objects.create_object(create_enemy());
            }

            m_grid_options = loaded.grid_options;
            m_scene_options = loaded.scene_options;
            m_cameras = loaded.cameras;
            m_player_controller.set_speed(loaded.player_speed);

            update_game_camera();

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