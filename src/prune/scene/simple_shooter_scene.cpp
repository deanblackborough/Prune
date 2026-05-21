#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/core/input.hpp"
#include "prune/scene/collision.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/scene/simple_shooter_concepts.hpp"
#include "prune/scene/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter_ids.hpp"
#include "prune/scene/simple_shooter_scene.hpp"
#include "prune/scene/simple_shooter_serializer.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    namespace {

        [[nodiscard]] const char* object_type_label(GameObjectType type) noexcept
        {
            return type == GameObjectType::Runtime ? "Runtime" : "Authored";
        }

        [[nodiscard]] const char* bool_label(bool value) noexcept
        {
            return value ? "Yes" : "No";
        }

        [[nodiscard]] const char* direction_label(Direction direction) noexcept
        {
            switch (direction) {
            case Direction::Up:
                return "Up";
            case Direction::Down:
                return "Down";
            case Direction::Left:
                return "Left";
            case Direction::Right:
                return "Right";
            }

            return "Unknown";
        }

        [[nodiscard]] GameObjectId first_object_id_for_kind(
            const SceneState& state,
            simple_shooter_concepts::ObjectKind kind
        ) noexcept
        {
            for (const auto& object : state.objects.objects()) {
                if (simple_shooter_concepts::kind_for(object) == kind) {
                    return object.identity.id;
                }
            }

            return k_invalid_game_object_id;
        }

        [[nodiscard]] bool object_has_kind(
            const SceneState& state,
            GameObjectId id,
            simple_shooter_concepts::ObjectKind kind
        ) noexcept
        {
            const GameObject* object = state.objects.get_by_id(id);
            return object && simple_shooter_concepts::kind_for(*object) == kind;
        }

        void restore_legacy_wall_concepts(SceneState& state)
        {
            for (auto& object : state.objects.objects()) {
                if (object.runtime.behaviour.empty() && object.collision.solid) {
                    object.runtime.behaviour = simple_shooter_ids::wall_behaviour;
                }
            }
        }

        void restore_loaded_shooter_concepts(SceneState& state, SimpleShooterState& shooter_state)
        {
            if (GameObject* player = state.objects.get_by_id(shooter_state.player_id)) {
                if (player->runtime.behaviour.empty()) {
                    player->runtime.behaviour = simple_shooter_ids::player_behaviour;
                }
            }

            restore_legacy_wall_concepts(state);

            if (!object_has_kind(state, shooter_state.player_id, simple_shooter_concepts::ObjectKind::Player)) {
                shooter_state.player_id = first_object_id_for_kind(state, simple_shooter_concepts::ObjectKind::Player);
            }

            if (shooter_state.player_id == k_invalid_game_object_id) {
                shooter_state.player_id = state.objects.create_object(simple_shooter_factory::create_player());
            }

            if (!object_has_kind(state, shooter_state.enemy_spawn_id, simple_shooter_concepts::ObjectKind::Spawn)) {
                shooter_state.enemy_spawn_id = first_object_id_for_kind(state, simple_shooter_concepts::ObjectKind::Spawn);
            }

            if (shooter_state.enemy_spawn_id == k_invalid_game_object_id) {
                shooter_state.enemy_spawn_id = state.objects.create_object(simple_shooter_factory::create_enemy_spawn());
            }

            if (!object_has_kind(state, shooter_state.enemy_id, simple_shooter_concepts::ObjectKind::Enemy)) {
                shooter_state.enemy_id = first_object_id_for_kind(state, simple_shooter_concepts::ObjectKind::Enemy);
            }

            if (shooter_state.enemy_id == k_invalid_game_object_id) {
                shooter_state.enemy_id = state.objects.create_object(simple_shooter_factory::create_enemy());
            }
        }
    }

    SimpleShooterScene::SimpleShooterScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }


    SceneState& SimpleShooterScene::get_state() noexcept
    {
        return m_state;
    }

    const SceneState& SimpleShooterScene::get_state() const noexcept
    {
        return m_state;
    }


    void SimpleShooterScene::on_enter()
    {
        new_scene();
    }

    void SimpleShooterScene::on_exit()
    {
        m_state.objects.clear();
        m_simple_shooter_state.player_id = k_invalid_game_object_id;
        m_simple_shooter_state.enemy_id = k_invalid_game_object_id;
        m_simple_shooter_state.enemy_spawn_id = k_invalid_game_object_id;
    }


    void SimpleShooterScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::scene_panel();

        if (ImGui::Begin("Simple Shooter", &open)) {
            if (ImGui::Button("Add Wall")) {
                create_wall_at_view_center();
            }

            ImGui::Separator();

            m_simple_shooter_tools.draw(m_simple_shooter_state);
        }

        ImGui::End();
    }

    std::string SimpleShooterScene::object_role_label(const GameObject& object) const
    {
        return simple_shooter_concepts::label(simple_shooter_concepts::kind_for(object));
    }

    SimpleShooterOptions& SimpleShooterScene::get_simple_shooter_options() noexcept
    {
        return m_simple_shooter_state.options;
    }

    const SimpleShooterOptions& SimpleShooterScene::get_simple_shooter_options() const noexcept
    {
        return m_simple_shooter_state.options;
    }

    SimpleShooterState& SimpleShooterScene::get_simple_shooter_state() noexcept
    {
        return m_simple_shooter_state;
    }

    const SimpleShooterState& SimpleShooterScene::get_simple_shooter_state() const noexcept
    {
        return m_simple_shooter_state;
    }

    GameObject* SimpleShooterScene::enemy_object() noexcept
    {
        return m_simple_shooter.enemy_object(m_state, m_simple_shooter_state);
    }

    const GameObject* SimpleShooterScene::enemy_object() const noexcept
    {
        return m_simple_shooter.enemy_object(m_state, m_simple_shooter_state);
    }

    int SimpleShooterScene::projectile_count() const noexcept
    {
        return m_simple_shooter.projectile_count(m_state);
    }

    void SimpleShooterScene::reset_simple_shooter()
    {
        m_simple_shooter.reset(m_state, m_simple_shooter_state);
    }

    void SimpleShooterScene::update_runtime(float dt, const Input& input, bool keyboard_input_enabled)
    {
        m_simple_shooter.update(
            m_state,
            m_camera,
            m_simple_shooter_state,
            dt,
            input,
            keyboard_input_enabled
        );
    }

    GameObject* SimpleShooterScene::game_camera_target() noexcept
    {
        return player_object();
    }

    void SimpleShooterScene::render_overlay(SDL_Renderer* renderer)
    {
        draw_player_facing_indicator(renderer);
    }

    GameObject* SimpleShooterScene::player_object() noexcept
    {
        return m_state.objects.get_by_id(m_simple_shooter_state.player_id);
    }

    const GameObject* SimpleShooterScene::player_object() const noexcept
    {
        return m_state.objects.get_by_id(m_simple_shooter_state.player_id);
    }

    void SimpleShooterScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_simple_shooter_state.player_id = k_invalid_game_object_id;
        m_simple_shooter_state.enemy_id = k_invalid_game_object_id;
        m_simple_shooter_state.enemy_spawn_id = k_invalid_game_object_id;

        m_camera.reset();

        m_grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};

        m_simple_shooter_state = {};
    }

    void SimpleShooterScene::restore_defaults()
    {
        reset_runtime_state();

        m_simple_shooter_state.player_id =
            m_state.objects.create_object(simple_shooter_factory::create_player());

        m_state.objects.create_object(simple_shooter_factory::create_wall());

        m_simple_shooter_state.enemy_spawn_id =
            m_state.objects.create_object(simple_shooter_factory::create_enemy_spawn());

        m_simple_shooter_state.enemy_id =
            m_state.objects.create_object(simple_shooter_factory::create_enemy());

        m_simple_shooter.reset(m_state, m_simple_shooter_state);

        m_state.objects.select(m_simple_shooter_state.player_id);

        m_camera.update_game_camera(m_state.viewport, player_object());
    }

    void SimpleShooterScene::new_scene()
    {
        restore_defaults();

        m_camera.update_game_camera(m_state.viewport, player_object());
    }

    bool SimpleShooterScene::save_to_file(std::string_view path, std::string& error) const
    {
        try {
            YAML::Node root;

            root["scene_type"] = "simple_shooter";

            SceneSerializer::save_to_node(m_state, m_camera, m_grid_options, root);
            SimpleShooterSerializer::save_to_node(m_simple_shooter_state, root);

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

    bool SimpleShooterScene::load_from_file(std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            if (!root["scene_type"] || root["scene_type"].as<std::string>() != "simple_shooter") {
                error = "Save file is not a Simple Shooter scene.";
                return false;
            }

            SceneState loaded_state = m_state;
            SceneCamera loaded_camera = m_camera;
            GridOptions loaded_grid_options = m_grid_options;
            SimpleShooterState loaded_simple_shooter_state{};

            if (!SceneSerializer::load_from_node(loaded_state, loaded_camera, loaded_grid_options, root, error)) {
                return false;
            }

            if (!SimpleShooterSerializer::load_from_node(root, loaded_simple_shooter_state, error)) {
                return false;
            }

            restore_loaded_shooter_concepts(loaded_state, loaded_simple_shooter_state);

            m_state = std::move(loaded_state);
            m_camera = loaded_camera;
            m_grid_options = loaded_grid_options;
            m_simple_shooter_state = loaded_simple_shooter_state;

            m_simple_shooter.reset(m_state, m_simple_shooter_state);
            m_camera.update_game_camera(m_state.viewport, player_object());

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

    void SimpleShooterScene::draw_scene_inspector(GameObject& selected)
    {
        if (!tooling::imgui::layout::collapsing_header("Simple Shooter", true)) {
            return;
        }

        if (!tooling::imgui::property_table::begin("##simple_shooter_inspector")) {
            return;
        }

        const auto kind = simple_shooter_concepts::kind_for(selected);

        tooling::imgui::property_table::text("Concept", simple_shooter_concepts::label(kind));
        tooling::imgui::property_table::text("Object Type", object_type_label(selected.identity.type));
        tooling::imgui::property_table::text("Runtime Saved", bool_label(selected.runtime.persistent));
        tooling::imgui::property_table::text_wrapped("Purpose", simple_shooter_concepts::purpose(kind));
        tooling::imgui::property_table::text_wrapped("Collision", simple_shooter_concepts::collision_rule(kind));

        if (kind == simple_shooter_concepts::ObjectKind::Player) {
            tooling::imgui::property_table::text("Facing", direction_label(selected.motion.facing));
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Projectile) {
            char lifetime_buffer[32];
            std::snprintf(lifetime_buffer, sizeof(lifetime_buffer), "%.2fs", selected.lifecycle.remaining);
            tooling::imgui::property_table::text("Remaining", lifetime_buffer);
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Wall) {
            tooling::imgui::property_table::text("Solid", bool_label(selected.collision.solid));
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Spawn) {
            tooling::imgui::property_table::text("Used By", "Enemy reset loop");
        }

        tooling::imgui::property_table::end();
    }

    GameObjectId SimpleShooterScene::create_wall_at_view_center()
    {
        GameObject wall = simple_shooter_factory::create_wall();

        wall.transform = find_wall_spawn_position(wall);

        const GameObjectId id = m_state.objects.create_object(wall);

        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Wall " + std::to_string(id);
        }

        m_state.objects.select(id);

        return id;
    }

    Transform SimpleShooterScene::find_wall_spawn_position(const GameObject& wall) const
    {
        const Camera& camera = m_camera.active();

        const float view_center_x =
            camera.x + (static_cast<float>(m_state.viewport.width) / camera.zoom) * 0.5f;

        const float view_center_y =
            camera.y + (static_cast<float>(m_state.viewport.height) / camera.zoom) * 0.5f;

        const int grid_size = m_grid_options.grid_size > 0
            ? m_grid_options.grid_size
            : wall.size.width;

        auto snap = [grid_size](float value) {
            return static_cast<float>(
                static_cast<int>(value / static_cast<float>(grid_size)) * grid_size
            );
        };

        const float base_x = m_grid_options.snap_to_grid
            ? snap(view_center_x - static_cast<float>(wall.size.width) * 0.5f)
            : view_center_x - static_cast<float>(wall.size.width) * 0.5f;

        const float base_y = m_grid_options.snap_to_grid
            ? snap(view_center_y - static_cast<float>(wall.size.height) * 0.5f)
            : view_center_y - static_cast<float>(wall.size.height) * 0.5f;

        constexpr int max_radius = 20;

        for (int radius = 0; radius <= max_radius; ++radius) {
            for (int y = -radius; y <= radius; ++y) {
                for (int x = -radius; x <= radius; ++x) {
                    if (std::abs(x) != radius && std::abs(y) != radius) {
                        continue;
                    }

                    GameObject candidate = wall;
                    candidate.transform.x = base_x + static_cast<float>(x * grid_size);
                    candidate.transform.y = base_y + static_cast<float>(y * grid_size);

                    if (is_space_free(candidate)) {
                        return candidate.transform;
                    }
                }
            }
        }

        return Transform{ base_x, base_y };
    }

    void SimpleShooterScene::draw_player_facing_indicator(SDL_Renderer* renderer) const
    {
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

        const int screen_x = static_cast<int>((indicator_x - camera.x) * camera.zoom);
        const int screen_y = static_cast<int>((indicator_y - camera.y) * camera.zoom);

        const int size = std::max(2, static_cast<int>(3.0f * camera.zoom));

        SDL_Point points[4]{};

        switch (player->motion.facing) {
        case Direction::Left:
            points[0] = { screen_x - size, screen_y };
            points[1] = { screen_x + size, screen_y - size };
            points[2] = { screen_x + size, screen_y + size };
            points[3] = points[0];
            break;

        case Direction::Right:
            points[0] = { screen_x + size, screen_y };
            points[1] = { screen_x - size, screen_y - size };
            points[2] = { screen_x - size, screen_y + size };
            points[3] = points[0];
            break;

        case Direction::Up:
            points[0] = { screen_x, screen_y - size };
            points[1] = { screen_x - size, screen_y + size };
            points[2] = { screen_x + size, screen_y + size };
            points[3] = points[0];
            break;

        case Direction::Down:
            points[0] = { screen_x, screen_y + size };
            points[1] = { screen_x - size, screen_y - size };
            points[2] = { screen_x + size, screen_y - size };
            points[3] = points[0];
            break;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLines(renderer, points, 4);
    }

    bool SimpleShooterScene::is_space_free(const GameObject& candidate) const noexcept
    {
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
}
