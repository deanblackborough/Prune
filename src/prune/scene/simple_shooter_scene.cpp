#include <cmath>
#include <fstream>
#include <string>
#include <utility>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/core/input.hpp"
#include "prune/scene/collision.hpp"
#include "prune/scene/simple_shooter_scene.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/scene/simple_shooter_factory.hpp"
#include "prune/scene/simple_shooter_ids.hpp"
#include "prune/scene/simple_shooter_serializer.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    SimpleShooterScene::SimpleShooterScene(int window_width, int window_height)
        : EditorScene(window_width, window_height)
    {
    }

    SceneCamera& SimpleShooterScene::get_camera() noexcept
    {
        return m_camera;
    }

    const SceneCamera& SimpleShooterScene::get_camera() const noexcept
    {
        return m_camera;
    }

    GridOptions& SimpleShooterScene::get_grid_options()
    {
        return m_grid_options;
    }

    void SimpleShooterScene::on_scene_exit()
    {
        m_simple_shooter_state.player_id = k_invalid_game_object_id;
        m_simple_shooter_state.enemy_id = k_invalid_game_object_id;
    }

    void SimpleShooterScene::render_scene_overlay(SDL_Renderer* renderer)
    {
        draw_player_facing_indicator(renderer);
    }

    void SimpleShooterScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::scene_panel();

        if (ImGui::Begin("Simple Shooter", &open)) {
            if (ImGui::Button("Add Block")) {
                create_block_at_view_center();
            }

            ImGui::Separator();

            m_simple_shooter_tools.draw(m_simple_shooter_state);
        }

        ImGui::End();
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

    int SimpleShooterScene::bullet_count() const noexcept
    {
        return m_simple_shooter.bullet_count(m_state);
    }

    void SimpleShooterScene::reset_simple_shooter()
    {
        m_simple_shooter.reset(m_state, m_simple_shooter_state);
    }

    void SimpleShooterScene::update_scene(float dt, const Input& input)
    {
        m_simple_shooter.update(
            m_state,
            m_camera,
            m_simple_shooter_state,
            dt,
            input,
            scene_keyboard_input_enabled()
        );
    }

    GameObject* SimpleShooterScene::follow_target() noexcept
    {
        return player_object();
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
        reset_common_state();
        m_camera.reset();
        m_grid_options = {};
        m_simple_shooter_state = {};
    }

    void SimpleShooterScene::restore_defaults()
    {
        reset_runtime_state();

        m_simple_shooter_state.player_id =
            m_state.objects.create_object(simple_shooter_factory::create_player());

        m_state.objects.create_object(simple_shooter_factory::create_initial_block());

        m_simple_shooter_state.enemy_id =
            m_state.objects.create_object(simple_shooter_factory::create_enemy());

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
            SceneCamera loaded_camera{};
            GridOptions loaded_grid_options{};
            SimpleShooterState loaded_simple_shooter_state{};

            if (!SceneSerializer::load_from_node(loaded_state, loaded_camera, loaded_grid_options, root, error)) {
                return false;
            }

            if (!SimpleShooterSerializer::load_from_node(root, loaded_simple_shooter_state, error)) {
                return false;
            }

            loaded_simple_shooter_state.enemy_id = k_invalid_game_object_id;

            for (const auto& object : loaded_state.objects.objects()) {
                if (object.runtime.behaviour == simple_shooter_ids::enemy_behaviour) {
                    loaded_simple_shooter_state.enemy_id = object.identity.id;
                    break;
                }
            }

            if (loaded_simple_shooter_state.enemy_id == k_invalid_game_object_id) {
                loaded_simple_shooter_state.enemy_id =
                    loaded_state.objects.create_object(simple_shooter_factory::create_enemy());
            }

            m_state = std::move(loaded_state);
            m_camera = loaded_camera;
            m_grid_options = loaded_grid_options;
            m_simple_shooter_state = loaded_simple_shooter_state;

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
        (void) selected;
    }

    GameObjectId SimpleShooterScene::create_block_at_view_center()
    {
        GameObject block = simple_shooter_factory::create_initial_block();

        block.transform = find_block_spawn_position(block);

        const GameObjectId id = m_state.objects.create_object(block);

        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Block " + std::to_string(id);
        }

        m_state.objects.select(id);

        return id;
    }

    Transform SimpleShooterScene::find_block_spawn_position(const GameObject& block) const
    {
        const Camera& camera = m_camera.active();

        const float view_center_x =
            camera.x + (static_cast<float>(m_state.viewport.width) / camera.zoom) * 0.5f;

        const float view_center_y =
            camera.y + (static_cast<float>(m_state.viewport.height) / camera.zoom) * 0.5f;

        const int grid_size = m_grid_options.grid_size > 0
            ? m_grid_options.grid_size
            : block.size.width;

        auto snap = [grid_size](float value) {
            return static_cast<float>(
                static_cast<int>(value / static_cast<float>(grid_size)) * grid_size
                );
            };

        const float base_x = m_grid_options.snap_to_grid
            ? snap(view_center_x - static_cast<float>(block.size.width) * 0.5f)
            : view_center_x - static_cast<float>(block.size.width) * 0.5f;

        const float base_y = m_grid_options.snap_to_grid
            ? snap(view_center_y - static_cast<float>(block.size.height) * 0.5f)
            : view_center_y - static_cast<float>(block.size.height) * 0.5f;

        constexpr int max_radius = 20;

        for (int radius = 0; radius <= max_radius; ++radius) {
            for (int y = -radius; y <= radius; ++y) {
                for (int x = -radius; x <= radius; ++x) {
                    if (std::abs(x) != radius && std::abs(y) != radius) {
                        continue;
                    }

                    GameObject candidate = block;
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

