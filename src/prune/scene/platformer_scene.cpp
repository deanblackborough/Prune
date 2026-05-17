#include <fstream>
#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/scene/platformer_factory.hpp"
#include "prune/scene/platformer_ids.hpp"
#include "prune/scene/platformer_scene.hpp"
#include "prune/scene/platformer_serializer.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    PlatformerScene::PlatformerScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void PlatformerScene::on_enter()
    {
        new_scene();
    }

    void PlatformerScene::on_exit()
    {
        m_state.objects.clear();
        m_platformer_state.player_id = k_invalid_game_object_id;
    }

    void PlatformerScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    bool PlatformerScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    GameObjectManager& PlatformerScene::get_object_manager()
    {
        return m_state.objects;
    }

    GridOptions& PlatformerScene::get_grid_options()
    {
        return m_state.grid_options;
    }

    SceneOptions& PlatformerScene::get_scene_options()
    {
        return m_state.scene_options;
    }

    SceneCamera& PlatformerScene::get_camera() noexcept
    {
        return m_state.camera;
    }

    const SceneCamera& PlatformerScene::get_camera() const noexcept
    {
        return m_state.camera;
    }

    void PlatformerScene::update(float dt, const Input& input)
    {
        m_platformer.update(
            m_state,
            m_platformer_state,
            dt,
            input,
            scene_keyboard_input_enabled()
        );

        m_interaction.update(m_state, dt, input);
        m_state.camera.update_game_camera(m_state.viewport, player_object());
    }

    void PlatformerScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state);
    }

    void PlatformerScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_platformer_state = {};
        m_state.camera.reset();
        m_state.grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};
    }

    void PlatformerScene::restore_defaults()
    {
        reset_runtime_state();

        m_platformer_state.player_id = m_state.objects.create_object(platformer_factory::create_player());

        m_state.objects.create_object(platformer_factory::create_ground(0.0f, 160.0f, 288, 16, "Ground"));
        m_state.objects.create_object(platformer_factory::create_ground(96.0f, 112.0f, 64, 16, "Small Platform"));
        m_state.objects.create_object(platformer_factory::create_ground(192.0f, 80.0f, 64, 16, "High Platform"));
        m_state.objects.create_object(platformer_factory::create_hazard(144.0f, 144.0f));

        m_state.objects.select(m_platformer_state.player_id);
        m_state.camera.update_game_camera(m_state.viewport, player_object());
    }

    void PlatformerScene::new_scene()
    {
        restore_defaults();
    }

    void PlatformerScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::simple_shooter();

        if (ImGui::Begin("Platformer", &open)) {
            if (ImGui::Button("Add Platform")) {
                add_platform_at_view_center();
            }

            ImGui::Separator();

            m_platformer_tools.draw(m_platformer_state.options, m_platformer_state.player_grounded);
        }

        ImGui::End();
    }

    void PlatformerScene::draw_scene_inspector(GameObject& selected)
    {
        if (selected.identity.id != m_platformer_state.player_id) {
            return;
        }

        if (tooling::imgui::layout::collapsing_header("Platformer Player")) {
            if (tooling::imgui::property_table::begin("##platformer_player")) {
                tooling::imgui::property_table::slider_float("Move speed", "##platformer_player_move_speed", m_platformer_state.options.move_speed, 0.0f, 256.0f, "%.2f");
                tooling::imgui::property_table::slider_float("Jump velocity", "##platformer_player_jump_velocity", m_platformer_state.options.jump_velocity, 0.0f, 512.0f, "%.2f");
                tooling::imgui::property_table::end();
            }
        }
    }

    bool PlatformerScene::save_to_file(std::string_view path, std::string& error) const
    {
        try {
            YAML::Node root;
            root["scene_type"] = "platformer";

            SceneSerializer::save_to_node(m_state, root);
            PlatformerSerializer::save_to_node(m_platformer_state, root);

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

    bool PlatformerScene::load_from_file(std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            if (!root["scene_type"] || root["scene_type"].as<std::string>() != "platformer") {
                error = "Save file is not a Platformer scene.";
                return false;
            }

            SceneState loaded_state = m_state;
            PlatformerState loaded_platformer_state{};

            if (!SceneSerializer::load_from_node(loaded_state, root, error)) {
                return false;
            }

            if (!PlatformerSerializer::load_from_node(root, loaded_platformer_state, error)) {
                return false;
            }

            if (loaded_state.objects.get_by_id(loaded_platformer_state.player_id) == nullptr) {
                loaded_platformer_state.player_id = k_invalid_game_object_id;

                for (const auto& object : loaded_state.objects.objects()) {
                    if (object.runtime.behaviour == platformer_ids::player_behaviour) {
                        loaded_platformer_state.player_id = object.identity.id;
                        break;
                    }
                }
            }

            if (loaded_platformer_state.player_id == k_invalid_game_object_id) {
                error = "Platformer scene is missing its player object.";
                return false;
            }

            m_state = std::move(loaded_state);
            m_platformer_state = loaded_platformer_state;
            m_state.camera.update_game_camera(m_state.viewport, player_object());

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

    GameObject* PlatformerScene::player_object() noexcept
    {
        return m_state.objects.get_by_id(m_platformer_state.player_id);
    }

    const GameObject* PlatformerScene::player_object() const noexcept
    {
        return m_state.objects.get_by_id(m_platformer_state.player_id);
    }

    void PlatformerScene::add_platform_at_view_center()
    {
        GameObject platform = platformer_factory::create_ground(0.0f, 0.0f, 64, 16, "Platform");
        platform.transform = view_center_spawn_position(platform.size.width, platform.size.height);

        const GameObjectId id = m_state.objects.create_object(platform);
        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Platform " + std::to_string(id);
        }

        m_state.objects.select(id);
    }

    Transform PlatformerScene::view_center_spawn_position(int width, int height) const
    {
        const Camera& camera = m_state.camera.active();

        const float view_center_x = camera.x + (static_cast<float>(m_state.viewport.width) / camera.zoom) * 0.5f;
        const float view_center_y = camera.y + (static_cast<float>(m_state.viewport.height) / camera.zoom) * 0.5f;

        Transform transform{};
        transform.x = view_center_x - (static_cast<float>(width) * 0.5f);
        transform.y = view_center_y - (static_cast<float>(height) * 0.5f);

        if (!m_state.grid_options.snap_to_grid || m_state.grid_options.grid_size <= 0) {
            return transform;
        }

        const int grid_size = m_state.grid_options.grid_size;
        transform.x = static_cast<float>(static_cast<int>(transform.x / static_cast<float>(grid_size)) * grid_size);
        transform.y = static_cast<float>(static_cast<int>(transform.y / static_cast<float>(grid_size)) * grid_size);
        return transform;
    }

}
