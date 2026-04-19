#include "prune/scene/sandbox_scene.hpp"

#include <fstream>
#include <sstream>

#include <yaml-cpp/yaml.h>

namespace prune {

    namespace {

        YAML::Node make_object_node(const GameObject& object)
        {
            YAML::Node node;
            node["id"] = object.id;
            node["name"] = object.name;

            node["transform"]["x"] = object.transform.x;
            node["transform"]["y"] = object.transform.y;

            node["rectangle"]["width"] = object.rectangle.width;
            node["rectangle"]["height"] = object.rectangle.height;

            YAML::Node colour;
            colour.push_back(object.rectangle.color[0]);
            colour.push_back(object.rectangle.color[1]);
            colour.push_back(object.rectangle.color[2]);
            node["colour"] = colour;

            node["active"] = object.active;
            node["visible"] = object.visible;
            node["solid"] = object.solid;
            node["is_player"] = object.is_player;

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

            const YAML::Node rectangle = node["rectangle"];
            if (!rectangle || !rectangle.IsMap()) {
                error = "Object is missing rectangle.";
                return false;
            }

            if (!read_required_int(rectangle, "width", object.rectangle.width) ||
                !read_required_int(rectangle, "height", object.rectangle.height)) {
                error = "Object rectangle is incomplete.";
                return false;
            }

            const YAML::Node colour = node["colour"];
            if (!colour || !colour.IsSequence() || colour.size() != 3) {
                error = "Object colour must be a 3-item sequence.";
                return false;
            }

            object.rectangle.color[0] = colour[0].as<float>();
            object.rectangle.color[1] = colour[1].as<float>();
            object.rectangle.color[2] = colour[2].as<float>();

            if (!read_required_bool(node, "active", object.active) ||
                !read_required_bool(node, "visible", object.visible) ||
                !read_required_bool(node, "solid", object.solid) ||
                !read_required_bool(node, "is_player", object.is_player)) {
                error = "Object flags are incomplete.";
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

            if (m_objects.selected_id() != kInvalidGameObjectId) {
                root["scene"]["selected_object_id"] = m_objects.selected_id();
            }

            root["cameras"]["mode"] = (m_cameras.mode == CameraMode::Editor) ? "editor" : "game";

            root["cameras"]["editor"]["x"] = m_cameras.editor.x;
            root["cameras"]["editor"]["y"] = m_cameras.editor.y;
            root["cameras"]["editor"]["speed"] = m_cameras.editor.speed;

            root["cameras"]["game"]["x"] = m_cameras.game.x;
            root["cameras"]["game"]["y"] = m_cameras.game.y;
            root["cameras"]["game"]["speed"] = m_cameras.game.speed;
            root["cameras"]["game"]["follow_player"] = m_cameras.game_options.follow_player;

            root["grid"]["show_grid"] = m_grid_options.show_grid;
            root["grid"]["snap_to_grid"] = m_grid_options.snap_to_grid;
            root["grid"]["grid_size"] = m_grid_options.grid_size;
            root["grid"]["nudge_step"] = m_grid_options.nudge_step;
            root["grid"]["shift_nudge_steps"] = m_grid_options.shift_nudge_steps;

            root["options"]["highlight_selected"] = m_scene_options.highlight_selected;

            YAML::Node objects = YAML::Node(YAML::NodeType::Sequence);
            for (const auto& object : m_objects.objects()) {
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
            const YAML::Node legacy_camera = root["camera"];
            const YAML::Node grid = root["grid"];
            const YAML::Node options = root["options"];
            const YAML::Node objects = root["objects"];

            if (!scene || !grid || !options || !objects || !objects.IsSequence()) {
                error = "Save file is missing required top-level sections.";
                return false;
            }

            reset_runtime_state();

            GameObjectId loaded_next_id = 1;
            GameObjectId loaded_player_id = kInvalidGameObjectId;
            GameObjectId loaded_selected_id = kInvalidGameObjectId;

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

                if (!parse_camera_mode(cameras["mode"], m_cameras.mode)) {
                    error = "cameras.mode is invalid.";
                    return false;
                }

                if (!read_required_float(editor, "x", m_cameras.editor.x) ||
                    !read_required_float(editor, "y", m_cameras.editor.y) ||
                    !read_required_float(editor, "speed", m_cameras.editor.speed)) {
                    error = "cameras.editor is incomplete.";
                    return false;
                }

                if (!read_required_float(game, "x", m_cameras.game.x) ||
                    !read_required_float(game, "y", m_cameras.game.y) ||
                    !read_required_float(game, "speed", m_cameras.game.speed) ||
                    !read_required_bool(game, "follow_player", m_cameras.game_options.follow_player)) {
                    error = "cameras.game is incomplete.";
                    return false;
                }
            }
            else if (legacy_camera) {
                if (!read_required_float(legacy_camera, "x", m_cameras.editor.x) ||
                    !read_required_float(legacy_camera, "y", m_cameras.editor.y)) {
                    error = "legacy camera is incomplete.";
                    return false;
                }

                m_cameras.editor.speed = 256.0f;
                m_cameras.game.speed = 256.0f;
                m_cameras.mode = CameraMode::Editor;
                m_cameras.game_options.follow_player = true;
                update_game_camera();
            }
            else {
                error = "Save file is missing cameras section.";
                return false;
            }

            if (!read_required_bool(grid, "show_grid", m_grid_options.show_grid) ||
                !read_required_bool(grid, "snap_to_grid", m_grid_options.snap_to_grid) ||
                !read_required_int(grid, "grid_size", m_grid_options.grid_size) ||
                !read_required_int(grid, "nudge_step", m_grid_options.nudge_step) ||
                !read_required_int(grid, "shift_nudge_steps", m_grid_options.shift_nudge_steps)) {
                error = "grid is incomplete.";
                return false;
            }

            if (!read_required_bool(options, "highlight_selected", m_scene_options.highlight_selected)) {
                error = "options.highlight_selected is missing.";
                return false;
            }

            int player_count = 0;

            for (const auto& entry : objects) {
                GameObject object{};
                if (!load_object_from_node(entry, object, error)) {
                    return false;
                }

                if (!m_objects.add_loaded_object(object)) {
                    error = "Failed to restore object. Duplicate or invalid id.";
                    return false;
                }

                if (object.is_player) {
                    ++player_count;
                }
            }

            if (m_objects.empty()) {
                error = "Save file contains no objects.";
                return false;
            }

            if (player_count != 1) {
                error = "Save file must contain exactly one player object.";
                return false;
            }

            m_player_id = loaded_player_id;
            GameObject* player = m_objects.get_by_id(m_player_id);

            if (!player) {
                error = "Saved player_id does not exist.";
                return false;
            }

            if (!player->is_player) {
                error = "Saved player_id does not point to a player object.";
                return false;
            }

            m_objects.set_next_id(loaded_next_id);

            if (loaded_selected_id != kInvalidGameObjectId &&
                m_objects.get_by_id(loaded_selected_id) != nullptr) {
                m_objects.set_selected_id(loaded_selected_id);
            }
            else {
                m_objects.set_selected_id(m_player_id);
            }

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