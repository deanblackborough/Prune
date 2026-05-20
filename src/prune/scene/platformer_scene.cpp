#include <cstdio>
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

    namespace {

        [[nodiscard]] const char* object_type_label(GameObjectType type) noexcept
        {
            return type == GameObjectType::Runtime ? "Runtime" : "Authored";
        }

        [[nodiscard]] const char* bool_label(bool value) noexcept
        {
            return value ? "Yes" : "No";
        }

        [[nodiscard]] const char* platformer_role_label(const GameObject& object) noexcept
        {
            if (object.runtime.behaviour == platformer_ids::player_behaviour) {
                return "Player";
            }

            if (object.runtime.behaviour == platformer_ids::ground_behaviour) {
                return "Platform / Ground";
            }

            if (object.runtime.behaviour == platformer_ids::hazard_behaviour) {
                return "Hazard";
            }

            return "Scene Object";
        }

        [[nodiscard]] const char* platformer_effect_label(const GameObject& object) noexcept
        {
            if (object.runtime.behaviour == platformer_ids::player_behaviour) {
                return "Controlled by horizontal movement and jump input. The camera follows this object.";
            }

            if (object.runtime.behaviour == platformer_ids::ground_behaviour) {
                return "Solid platformer surface. The player collides with this and can stand on it.";
            }

            if (object.runtime.behaviour == platformer_ids::hazard_behaviour) {
                return "Resets the player to the platformer spawn point on contact.";
            }

            return "No Platformer behaviour is assigned.";
        }
    }

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




    void PlatformerScene::update_runtime(float dt, const Input& input, bool keyboard_input_enabled)
    {
        m_platformer.update(
            m_state,
            m_camera,
            m_platformer_state,
            dt,
            input,
            keyboard_input_enabled
        );
    }

    GameObject* PlatformerScene::game_camera_target() noexcept
    {
        return player_object();
    }

    void PlatformerScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_platformer_state = {};
        m_camera.reset();
        m_grid_options = {};
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
        m_camera.update_game_camera(m_state.viewport, player_object());
    }

    void PlatformerScene::new_scene()
    {
        restore_defaults();
    }

    void PlatformerScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::scene_panel();

        if (ImGui::Begin("Platformer", &open)) {
            if (ImGui::Button("Add Platform")) {
                add_platform_at_view_center();
            }

            ImGui::SameLine();

            if (ImGui::Button("Add Hazard")) {
                add_hazard_at_view_center();
            }

            ImGui::Separator();

            m_platformer_tools.draw(m_platformer_state.options, m_platformer_state.player_grounded);
        }

        ImGui::End();
    }

    void PlatformerScene::draw_scene_inspector(GameObject& selected)
    {
        if (!tooling::imgui::layout::collapsing_header("Platformer", true)) {
            return;
        }

        if (!tooling::imgui::property_table::begin("##platformer_inspector")) {
            return;
        }

        tooling::imgui::property_table::text("Role", platformer_role_label(selected));
        tooling::imgui::property_table::text("Object Type", object_type_label(selected.identity.type));
        tooling::imgui::property_table::text("Behaviour", selected.runtime.behaviour.empty() ? "None" : selected.runtime.behaviour.c_str());
        tooling::imgui::property_table::text("Runtime Saved", bool_label(selected.runtime.persistent));
        tooling::imgui::property_table::text_wrapped("Effect", platformer_effect_label(selected));

        if (selected.runtime.behaviour == platformer_ids::player_behaviour) {
            char velocity_buffer[64];
            std::snprintf(
                velocity_buffer,
                sizeof(velocity_buffer),
                "x %.1f, y %.1f",
                selected.motion.velocity.x,
                selected.motion.velocity.y
            );

            tooling::imgui::property_table::text("Velocity", velocity_buffer);
            tooling::imgui::property_table::text("Grounded", bool_label(m_platformer_state.player_grounded));
        }
        else if (selected.runtime.behaviour == platformer_ids::ground_behaviour) {
            tooling::imgui::property_table::text("Collision", selected.collision.solid ? "Solid" : "Not solid");
        }
        else if (selected.runtime.behaviour == platformer_ids::hazard_behaviour) {
            tooling::imgui::property_table::text("Collision", selected.collision.solid ? "Solid" : "Trigger only");
        }

        tooling::imgui::property_table::end();
    }

    bool PlatformerScene::save_to_file(std::string_view path, std::string& error) const
    {
        try {
            YAML::Node root;
            root["scene_type"] = "platformer";

            SceneSerializer::save_to_node(m_state, m_camera, m_grid_options, root);
            PlatformerSerializer::save_to_node(m_platformer_state, root);

            std::ofstream output{ std::string(path) };
            if (!output.is_open()) {
                error = "Could not open save file for writing.";
                return false;
            }

            output << root;

            if (!output) {
                error = "Failed to write scene data to file.";
                return false;
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

    bool PlatformerScene::load_from_file(std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            if (!root["scene_type"] || root["scene_type"].as<std::string>() != "platformer") {
                error = "Save file is not a Platformer scene.";
                return false;
            }

            SceneState loaded_state = m_state;
            SceneCamera loaded_camera = m_camera;
            GridOptions loaded_grid_options = m_grid_options;
            PlatformerState loaded_platformer_state{};

            if (!SceneSerializer::load_from_node(loaded_state, loaded_camera, loaded_grid_options, root, error)) {
                return false;
            }

            if (!PlatformerSerializer::load_from_node(root, loaded_platformer_state, error)) {
                return false;
            }

            const GameObject* loaded_player = loaded_state.objects.get_by_id(loaded_platformer_state.player_id);
            if (loaded_player == nullptr || loaded_player->runtime.behaviour != platformer_ids::player_behaviour) {
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
            m_camera = loaded_camera;
            m_grid_options = loaded_grid_options;
            m_platformer_state = loaded_platformer_state;
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

    void PlatformerScene::add_hazard_at_view_center()
    {
        GameObject hazard = platformer_factory::create_hazard(0.0f, 0.0f);
        hazard.transform = view_center_spawn_position(hazard.size.width, hazard.size.height);

        const GameObjectId id = m_state.objects.create_object(hazard);
        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Hazard " + std::to_string(id);
        }

        m_state.objects.select(id);
    }

    Transform PlatformerScene::view_center_spawn_position(int width, int height) const
    {
        const Camera& camera = m_camera.active();

        const float view_center_x = camera.x + (static_cast<float>(m_state.viewport.width) / camera.zoom) * 0.5f;
        const float view_center_y = camera.y + (static_cast<float>(m_state.viewport.height) / camera.zoom) * 0.5f;

        Transform transform{};
        transform.x = view_center_x - (static_cast<float>(width) * 0.5f);
        transform.y = view_center_y - (static_cast<float>(height) * 0.5f);

        if (!m_grid_options.snap_to_grid || m_grid_options.grid_size <= 0) {
            return transform;
        }

        const int grid_size = m_grid_options.grid_size;
        const float grid = static_cast<float>(grid_size);
        transform.x = std::floor(transform.x / grid) * grid;
        transform.y = std::floor(transform.y / grid) * grid;
        return transform;
    }

}
