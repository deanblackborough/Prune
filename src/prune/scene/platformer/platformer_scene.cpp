#include <array>
#include <cstdio>
#include <string>

#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/scene/platformer/platformer_concepts.hpp"
#include "prune/scene/platformer/platformer_factory.hpp"
#include "prune/scene/platformer/platformer_scene.hpp"
#include "prune/scene/platformer/platformer_serializer.hpp"
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

            if (GameObject* player = state.objects.get_by_id(platformer_state.player_id)) {
                player->editor.movable = true;
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
        // Scene activation hook only. Default content is created explicitly via new_scene().
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
        m_state.editor_tool = EditorTool::Select;
        m_state.editor_commands.clear();
    }

    void PlatformerScene::restore_defaults()
    {
        reset_runtime_state();

        m_platformer_state.player_start_id =
            m_state.objects.create_object(platformer_factory::create_player_start(32.0f, 112.0f));

        m_platformer_state.player_id =
            m_state.objects.create_object(platformer_factory::create_player());

        m_state.objects.create_object(platformer_factory::create_platform(0.0f, 176.0f, 400, 16, "Main Platform"));
        m_state.objects.create_object(platformer_factory::create_platform(96.0f, 144.0f, 80, 16, "Lower Step"));
        m_state.objects.create_object(platformer_factory::create_platform(208.0f, 112.0f, 80, 16, "Mid Platform"));
        m_state.objects.create_object(platformer_factory::create_platform(320.0f, 80.0f, 64, 16, "High Platform"));
        m_state.objects.create_object(platformer_factory::create_hazard(144.0f, 160.0f, 32, 16, "Spike Pit"));
        m_state.objects.create_object(platformer_factory::create_hazard(288.0f, 160.0f, 32, 16, "Second Spike Pit"));

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

    std::span<const SceneCreationAction> PlatformerScene::scene_creation_actions() const noexcept
    {
        static constexpr std::array<SceneCreationAction, 2> actions{ {
            { "platform", "Platform" },
            { "hazard", "Hazard" }
        } };

        return actions;
    }

    GameObjectId PlatformerScene::create_scene_object(std::string_view action_id)
    {
        if (action_id == "platform") {
            return add_platform_at_view_center();
        }

        if (action_id == "hazard") {
            return add_hazard_at_view_center();
        }

        return k_invalid_game_object_id;
    }

    void PlatformerScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::scene_panel();

        if (ImGui::Begin("Platformer", &open)) {
            draw_debug_tools();

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
        else if (kind == platformer_concepts::ObjectKind::Platform) {
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

    std::string_view PlatformerScene::default_file_path() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::Platformer)) {
            return descriptor->default_file_path;
        }

        return {};
    }

    std::string_view PlatformerScene::scene_type_id() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::Platformer)) {
            return descriptor->id;
        }

        return {};
    }

    void PlatformerScene::save_scene_data(YAML::Node& root) const
    {
        PlatformerSerializer::save_to_node(m_platformer_state, root);
    }

    bool PlatformerScene::load_scene_data(const YAML::Node& root, std::string& error)
    {
        PlatformerState loaded_platformer_state{};

        if (!PlatformerSerializer::load_from_node(root, loaded_platformer_state, error)) {
            return false;
        }

        m_platformer_state = loaded_platformer_state;
        return true;
    }

    bool PlatformerScene::restore_loaded_scene(SceneState& state, std::string& error)
    {
        restore_loaded_platformer_concepts(state, m_platformer_state);

        if (m_platformer_state.player_id == k_invalid_game_object_id) {
            error = "Platformer scene is missing its player object.";
            return false;
        }

        return true;
    }

    GameObject* PlatformerScene::player_object() noexcept
    {
        return m_state.objects.get_by_id(m_platformer_state.player_id);
    }

    const GameObject* PlatformerScene::player_object() const noexcept
    {
        return m_state.objects.get_by_id(m_platformer_state.player_id);
    }

    GameObjectId PlatformerScene::add_platform_at_view_center()
    {
        GameObject platform = platformer_factory::create_platform(0.0f, 0.0f, 64, 16, "Platform");
        platform.transform = first_free_view_center_spawn_position(platform);

        const GameObjectId id = m_state.objects.create_object(platform);
        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Platform " + std::to_string(id);
        }

        m_state.objects.select(id);
        return id;
    }

    GameObjectId PlatformerScene::add_hazard_at_view_center()
    {
        GameObject hazard = platformer_factory::create_hazard(0.0f, 0.0f);
        hazard.transform = first_free_view_center_spawn_position(hazard);

        const GameObjectId id = m_state.objects.create_object(hazard);
        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Hazard " + std::to_string(id);
        }

        m_state.objects.select(id);
        return id;
    }


}
