#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "prune/resources/sprites.hpp"
#include "prune/scene/scene_serializer.hpp"

namespace prune {

    namespace {

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
            node["id"] = object.identity.id;
            node["name"] = object.identity.name;
            node["type"] = to_string(object.identity.type);
            node["active"] = object.lifecycle.active;

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

            if (!read_required_uint(node, "id", object.identity.id)) {
                error = "Object id is missing.";
                return false;
            }

            object.identity.name = node["name"].as<std::string>();

            if (!parse_game_object_type(node["type"], object.identity.type)) {
                error = "Object type is invalid.";
                return false;
            }

            if (!read_required_bool(node, "active", object.lifecycle.active)) {
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

            object.motion.velocity = {};
            object.lifecycle.remaining = 0.0f;

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

    // ---- Save pieces ----

    void SceneSerializer::save_objects(const GameObjectManager& objects, YAML::Node& root)
    {
        root["scene"]["next_object_id"] = objects.next_id();

        if (objects.selected_id() != k_invalid_game_object_id) {
            root["scene"]["selected_object_id"] = objects.selected_id();
        }

        YAML::Node seq = YAML::Node(YAML::NodeType::Sequence);

        for (const auto& object : objects.objects()) {
            if (!object.runtime.persistent) {
                continue;
            }

            seq.push_back(make_object_node(object));
        }

        root["objects"] = seq;
    }

    void SceneSerializer::save_scene_options(const SceneOptions& options, YAML::Node& root)
    {
        root["options"]["highlight_selected"] = options.highlight_selected;
    }

    void SceneSerializer::save_camera(const SceneCamera& camera, YAML::Node& root)
    {
        root["cameras"]["mode"] = camera.state().mode == CameraMode::Editor ? "editor" : "game";

        root["cameras"]["editor"]["x"] = camera.state().editor.x;
        root["cameras"]["editor"]["y"] = camera.state().editor.y;
        root["cameras"]["editor"]["speed"] = camera.state().editor.speed;
        root["cameras"]["editor"]["zoom"] = camera.state().editor.zoom;

        root["cameras"]["game"]["x"] = camera.state().game.x;
        root["cameras"]["game"]["y"] = camera.state().game.y;
        root["cameras"]["game"]["speed"] = camera.state().game.speed;
        root["cameras"]["game"]["zoom"] = camera.state().game.zoom;
        root["cameras"]["game"]["follow_target"] = camera.state().game_options.follow_target;
    }

    void SceneSerializer::save_grid(const GridOptions& grid, YAML::Node& root)
    {
        root["grid"]["show_grid"] = grid.show_grid;
        root["grid"]["snap_to_grid"] = grid.snap_to_grid;
        root["grid"]["grid_size"] = grid.grid_size;
        root["grid"]["nudge_step"] = grid.nudge_step;
        root["grid"]["shift_nudge_steps"] = grid.shift_nudge_steps;
    }

    // ---- Load pieces ----

    bool SceneSerializer::load_objects(GameObjectManager& objects, const YAML::Node& root, std::string& error)
    {
        const YAML::Node scene = root["scene"];
        const YAML::Node seq = root["objects"];

        if (!scene || !seq || !seq.IsSequence()) {
            error = "Save file is missing required scene/objects sections.";
            return false;
        }

        GameObjectId loaded_next_id = 1;
        GameObjectId loaded_selected_id = k_invalid_game_object_id;

        if (!read_required_uint(scene, "next_object_id", loaded_next_id)) {
            error = "scene.next_object_id is missing.";
            return false;
        }

        if (scene["selected_object_id"]) {
            loaded_selected_id = scene["selected_object_id"].as<GameObjectId>();
        }

        GameObjectManager loaded{};

        for (const auto& entry : seq) {
            GameObject object{};

            if (!load_object_from_node(entry, object, error)) {
                return false;
            }

            if (!object.runtime.persistent) {
                error = "Save file contains a non-persistent object.";
                return false;
            }

            if (!loaded.add_loaded_object(object)) {
                error = "Failed to restore object. Duplicate or invalid id.";
                return false;
            }
        }

        if (loaded.empty()) {
            error = "Save file contains no objects.";
            return false;
        }

        GameObjectId max_loaded_id = k_invalid_game_object_id;
        for (const auto& object : loaded.objects()) {
            if (object.identity.id > max_loaded_id) {
                max_loaded_id = object.identity.id;
            }
        }

        if (loaded_next_id <= max_loaded_id) {
            loaded_next_id = max_loaded_id + 1;
        }

        loaded.set_next_id(loaded_next_id);

        if (loaded_selected_id != k_invalid_game_object_id &&
            loaded.get_by_id(loaded_selected_id) != nullptr) {
            loaded.set_selected_id(loaded_selected_id);
        } else if (!loaded.empty()) {
            loaded.set_selected_id(loaded.objects().front().identity.id);
        }

        objects = std::move(loaded);
        return true;
    }

    bool SceneSerializer::load_scene_options(SceneOptions& options, const YAML::Node& root, std::string& error)
    {
        const YAML::Node opts = root["options"];
        if (!opts) {
            error = "Save file is missing options section.";
            return false;
        }

        if (!read_required_bool(opts, "highlight_selected", options.highlight_selected)) {
            error = "options.highlight_selected is missing.";
            return false;
        }

        return true;
    }

    bool SceneSerializer::load_camera(SceneCamera& camera, const YAML::Node& root, std::string& error)
    {
        const YAML::Node cameras = root["cameras"];
        if (!cameras) {
            error = "Save file is missing cameras section.";
            return false;
        }

        const YAML::Node editor = cameras["editor"];
        const YAML::Node game = cameras["game"];

        if (!editor || !game) {
            error = "cameras section is incomplete.";
            return false;
        }

        CameraState loaded{};

        if (!parse_camera_mode(cameras["mode"], loaded.mode)) {
            error = "cameras.mode is invalid.";
            return false;
        }

        if (!read_required_float(editor, "x", loaded.editor.x) ||
            !read_required_float(editor, "y", loaded.editor.y) ||
            !read_required_float(editor, "speed", loaded.editor.speed) ||
            !read_required_float(editor, "zoom", loaded.editor.zoom)) {
            error = "cameras.editor is incomplete.";
            return false;
        }

        if (!read_required_float(game, "x", loaded.game.x) ||
            !read_required_float(game, "y", loaded.game.y) ||
            !read_required_float(game, "speed", loaded.game.speed) ||
            !read_required_float(game, "zoom", loaded.game.zoom) ||
            !read_required_bool(game, "follow_target", loaded.game_options.follow_target)) {
            error = "cameras.game is incomplete.";
            return false;
        }

        camera.state() = loaded;
        return true;
    }

    bool SceneSerializer::load_grid(GridOptions& grid, const YAML::Node& root, std::string& error)
    {
        const YAML::Node g = root["grid"];
        if (!g) {
            error = "Save file is missing grid section.";
            return false;
        }

        if (!read_required_bool(g, "show_grid", grid.show_grid) ||
            !read_required_bool(g, "snap_to_grid", grid.snap_to_grid) ||
            !read_required_int(g, "grid_size", grid.grid_size) ||
            !read_required_int(g, "nudge_step", grid.nudge_step) ||
            !read_required_int(g, "shift_nudge_steps", grid.shift_nudge_steps)) {
            error = "grid section is incomplete.";
            return false;
        }

        return true;
    }
}
