#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/core/input.hpp"
#include "prune/scene/simple_shooter/simple_shooter_concepts.hpp"
#include "prune/scene/simple_shooter/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter/simple_shooter_ids.hpp"
#include "prune/scene/simple_shooter/simple_shooter_scene.hpp"
#include "prune/scene/simple_shooter/simple_shooter_serializer.hpp"
#include "prune/scene/scene_factory.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    namespace {

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

                player->editor.movable = true;
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


    void SimpleShooterScene::on_enter()
    {
        // Scene activation hook only. Default content is created explicitly via new_scene().
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
            draw_debug_tools();

            ImGui::Separator();

            m_simple_shooter_tools.draw(m_simple_shooter_state);
        }

        ImGui::End();
    }

    ObjectConcept SimpleShooterScene::object_concept_for(const GameObject& object) const
    {
        return simple_shooter_concepts::describe_object(object);
    }

    std::span<const SceneCreationAction> SimpleShooterScene::scene_creation_actions() const noexcept
    {
        static constexpr std::array<SceneCreationAction, 1> actions{ {
            { "wall", "Wall" }
        } };

        return actions;
    }

    GameObjectId SimpleShooterScene::create_scene_object(std::string_view action_id)
    {
        if (action_id == "wall") {
            return create_wall_at_view_center();
        }

        return k_invalid_game_object_id;
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
        m_state.editor_tool = EditorTool::Select;
        m_state.editor_commands.clear();

        m_simple_shooter_state = {};
    }

    void SimpleShooterScene::restore_defaults()
    {
        reset_runtime_state();

        m_simple_shooter_state.player_id =
            m_state.objects.create_object(simple_shooter_factory::create_player());

        m_state.objects.create_object(
            simple_shooter_factory::create_wall(176.0f, 96.0f, 16, 64, "Central Wall")
        );
        m_state.objects.create_object(
            simple_shooter_factory::create_wall(80.0f, 192.0f, 96, 16, "Lower Cover")
        );
        m_state.objects.create_object(
            simple_shooter_factory::create_wall(224.0f, 64.0f, 96, 16, "Upper Cover")
        );

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

    std::string_view SimpleShooterScene::default_file_path() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::SimpleShooter)) {
            return descriptor->default_file_path;
        }

        return {};
    }

    std::string_view SimpleShooterScene::scene_type_id() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::SimpleShooter)) {
            return descriptor->id;
        }

        return {};
    }

    void SimpleShooterScene::save_scene_data(YAML::Node& root) const
    {
        SimpleShooterSerializer::save_to_node(m_simple_shooter_state, root);
    }

    bool SimpleShooterScene::load_scene_data(const YAML::Node& root, std::string& error)
    {
        SimpleShooterState loaded_simple_shooter_state{};

        if (!SimpleShooterSerializer::load_from_node(root, loaded_simple_shooter_state, error)) {
            return false;
        }

        m_simple_shooter_state = loaded_simple_shooter_state;
        return true;
    }

    bool SimpleShooterScene::restore_loaded_scene(SceneState& state, std::string&)
    {
        restore_loaded_shooter_concepts(state, m_simple_shooter_state);
        m_simple_shooter.reset(state, m_simple_shooter_state);
        return true;
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

        if (kind == simple_shooter_concepts::ObjectKind::Player) {
            tooling::imgui::property_table::text("Facing", direction_label(selected.motion.facing));
            tooling::imgui::property_table::text("Controller Speed", std::to_string(static_cast<int>(m_simple_shooter_state.player_controller.speed())).c_str());
            tooling::imgui::property_table::text("Fire Ready", bool_label(m_simple_shooter_state.fire_cooldown_remaining <= 0.0f));

            char cooldown_buffer[32];
            std::snprintf(cooldown_buffer, sizeof(cooldown_buffer), "%.2fs", m_simple_shooter_state.fire_cooldown_remaining);
            tooling::imgui::property_table::text("Cooldown Remaining", cooldown_buffer);
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Enemy) {
            tooling::imgui::property_table::text("Tracked Enemy", bool_label(selected.identity.id == m_simple_shooter_state.enemy_id));
            tooling::imgui::property_table::text("Enemy Speed", std::to_string(static_cast<int>(m_simple_shooter_state.options.enemy_speed)).c_str());
            tooling::imgui::property_table::text("Spawn Object Id", std::to_string(m_simple_shooter_state.enemy_spawn_id).c_str());
            tooling::imgui::property_table::text("Max Live Enemies", std::to_string(m_simple_shooter_state.options.max_live_enemies).c_str());
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Projectile) {
            char lifetime_buffer[32];
            std::snprintf(lifetime_buffer, sizeof(lifetime_buffer), "%.2fs", selected.lifecycle.remaining);

            tooling::imgui::property_table::text("Remaining", lifetime_buffer);
            tooling::imgui::property_table::text("Projectile Speed", std::to_string(static_cast<int>(m_simple_shooter_state.options.projectile_speed)).c_str());
            tooling::imgui::property_table::text("Configured Lifetime", std::to_string(m_simple_shooter_state.options.projectile_lifetime).c_str());
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Wall) {
            tooling::imgui::property_table::text("Blocks Player", bool_label(selected.collision.solid));
            tooling::imgui::property_table::text("Stops Projectiles", bool_label(selected.collision.solid));
        }
        else if (kind == simple_shooter_concepts::ObjectKind::Spawn) {
            tooling::imgui::property_table::text("Tracked Spawn", bool_label(selected.identity.id == m_simple_shooter_state.enemy_spawn_id));
            tooling::imgui::property_table::text("Used By", "Enemy respawn/reset loop");
            tooling::imgui::property_table::text("Enemy Object Id", std::to_string(m_simple_shooter_state.enemy_id).c_str());
        }
        else {
            tooling::imgui::property_table::text("Shooter Behaviour", "No Simple Shooter behaviour is assigned");
        }

        tooling::imgui::property_table::end();
    }

    GameObjectId SimpleShooterScene::create_wall_at_view_center()
    {
        GameObject wall = simple_shooter_factory::create_wall();

        wall.transform = first_free_view_center_spawn_position(wall);

        const GameObjectId id = m_state.objects.create_object(wall);

        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Wall " + std::to_string(id);
        }

        m_state.objects.select(id);

        return id;
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


}
