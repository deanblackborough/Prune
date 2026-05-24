#include <cmath>
#include <cstdio>
#include <fstream>
#include <string>
#include <utility>

#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/scene/platformer_concepts.hpp"
#include "prune/scene/platformer_factory.hpp"
#include "prune/scene/platformer_scene.hpp"
#include "prune/scene/platformer_serializer.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune {

    namespace {

        [[nodiscard]] const char* bool_label(bool value) noexcept
        {
            return value ? "Yes" : "No";
        }

        [[nodiscard]] GameObjectId first_object_id_for_kind(
            const SceneState& state,
            platformer_concepts::ObjectKind kind
        ) noexcept
        {
            for (const auto& object : state.objects.objects()) {
                if (platformer_concepts::kind_for(object) == kind) {
                    return object.identity.id;
                }
            }

            return k_invalid_game_object_id;
        }

        [[nodiscard]] bool object_has_kind(
            const SceneState& state,
            GameObjectId id,
            platformer_concepts::ObjectKind kind
        ) noexcept
        {
            const GameObject* object = state.objects.get_by_id(id);
            return object && platformer_concepts::kind_for(*object) == kind;
        }

        void restore_loaded_platformer_concepts(SceneState& state, PlatformerState& platformer_state)
        {
            if (!object_has_kind(state, platformer_state.player_id, platformer_concepts::ObjectKind::Player)) {
                platformer_state.player_id = first_object_id_for_kind(state, platformer_concepts::ObjectKind::Player);
            }

            if (!object_has_kind(state, platformer_state.player_start_id, platformer_concepts::ObjectKind::PlayerStart)) {
                platformer_state.player_start_id = first_object_id_for_kind(state, platformer_concepts::ObjectKind::PlayerStart);
            }

            if (platformer_state.player_start_id == k_invalid_game_object_id) {
                platformer_state.player_start_id = state.objects.create_object(platformer_factory::create_player_start(32.0f, 112.0f));
            }
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
        m_platformer_state.player_start_id = k_invalid_game_object_id;
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

        m_platformer_state.player_start_id =
            m_state.objects.create_object(platformer_factory::create_player_start(32.0f, 112.0f));

        m_platformer_state.player_id =
            m_state.objects.create_object(platformer_factory::create_player());

        m_state.objects.create_object(platformer_factory::create_ground(0.0f, 176.0f, 400, 16, "Main Ground"));
        m_state.objects.create_object(platformer_factory::create_ground(96.0f, 144.0f, 80, 16, "Lower Step"));
        m_state.objects.create_object(platformer_factory::create_ground(208.0f, 112.0f, 80, 16, "Mid Platform"));
        m_state.objects.create_object(platformer_factory::create_ground(320.0f, 80.0f, 64, 16, "High Platform"));
        m_state.objects.create_object(platformer_factory::create_hazard(144.0f, 160.0f, 32, 16, "Spike Pit"));
        m_state.objects.create_object(platformer_factory::create_hazard(288.0f, 160.0f, 32, 16, "Exit Pit"));

        m_platformer.reset_player(m_state, m_platformer_state);
        m_state.objects.select(m_platformer_state.player_id);

        m_camera.game().zoom = 3.0f;
        m_camera.editor().zoom = 3.0f;
        m_camera.update_game_camera(m_state.viewport, player_object());
        m_camera.editor().x = m_camera.game().x;
        m_camera.editor().y = m_camera.game().y;
    }

    void PlatformerScene::new_scene()
    {
        restore_defaults();
    }

    ObjectConcept PlatformerScene::object_concept_for(const GameObject& object) const
    {
        return platformer_concepts::describe_object(object);
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

            m_platformer_tools.draw(m_platformer_state);
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

        const auto kind = platformer_concepts::kind_for(selected);

        if (kind == platformer_concepts::ObjectKind::Player) {
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
            tooling::imgui::property_table::text("Move Speed", std::to_string(static_cast<int>(m_platformer_state.options.move_speed)).c_str());
            tooling::imgui::property_table::text("Jump Velocity", std::to_string(static_cast<int>(m_platformer_state.options.jump_velocity)).c_str());
            tooling::imgui::property_table::text("Gravity", std::to_string(static_cast<int>(m_platformer_state.options.gravity)).c_str());
            tooling::imgui::property_table::text("Reset Target Id", std::to_string(m_platformer_state.player_start_id).c_str());
        }
        else if (kind == platformer_concepts::ObjectKind::PlayerStart) {
            tooling::imgui::property_table::text("Tracked Reset Marker", bool_label(selected.identity.id == m_platformer_state.player_start_id));
            tooling::imgui::property_table::text("Used By", "Hazard and fall reset");
            tooling::imgui::property_table::text("Player Object Id", std::to_string(m_platformer_state.player_id).c_str());
        }
        else if (kind == platformer_concepts::ObjectKind::Ground) {
            tooling::imgui::property_table::text("Collision Surface", bool_label(selected.collision.solid));
            tooling::imgui::property_table::text("Can Ground Player", bool_label(selected.collision.solid));
        }
        else if (kind == platformer_concepts::ObjectKind::Hazard) {
            tooling::imgui::property_table::text("Resets Player", "On contact");
            tooling::imgui::property_table::text("Reset Target Id", std::to_string(m_platformer_state.player_start_id).c_str());
        }
        else {
            tooling::imgui::property_table::text("Platformer Behaviour", "No Platformer behaviour is assigned");
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

            restore_loaded_platformer_concepts(loaded_state, loaded_platformer_state);

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
