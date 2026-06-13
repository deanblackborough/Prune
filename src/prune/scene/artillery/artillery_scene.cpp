#include <array>
#include <cmath>
#include <numbers>
#include <string>

#include <SDL2/SDL.h>
#include <yaml-cpp/yaml.h>

#include "imgui.h"

#include "prune/core/defaults.hpp"
#include "prune/scene/artillery/artillery_concepts.hpp"
#include "prune/scene/artillery/artillery_factory.hpp"
#include "prune/scene/artillery/artillery_scene.hpp"
#include "prune/scene/artillery/artillery_serializer.hpp"
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

        [[nodiscard]] const char* turn_label(ArtilleryTurn turn) noexcept
        {
            return turn == ArtilleryTurn::PlayerTwo ? "Player Two" : "Player One";
        }

        [[nodiscard]] GameObjectId first_object_id_for_kind(const SceneState& state, artillery_concepts::ObjectKind kind) noexcept
        {
            for (const auto& object : state.objects.objects()) {
                if (artillery_concepts::kind_for(object) == kind) {
                    return object.identity.id;
                }
            }

            return k_invalid_game_object_id;
        }

        [[nodiscard]] bool object_has_kind(const SceneState& state, GameObjectId id, artillery_concepts::ObjectKind kind) noexcept
        {
            const GameObject* object = state.objects.get_by_id(id);
            return object && artillery_concepts::kind_for(*object) == kind;
        }

        void restore_loaded_artillery_concepts(SceneState& state, ArtilleryState& artillery_state)
        {
            if (!object_has_kind(state, artillery_state.player_one_id, artillery_concepts::ObjectKind::Tank)) {
                artillery_state.player_one_id = first_object_id_for_kind(state, artillery_concepts::ObjectKind::Tank);
            }

            if (!object_has_kind(state, artillery_state.player_two_id, artillery_concepts::ObjectKind::Tank)) {
                bool skipped_first_tank = false;

                for (const auto& object : state.objects.objects()) {
                    if (!artillery_concepts::is_tank(object)) {
                        continue;
                    }

                    if (!skipped_first_tank && object.identity.id == artillery_state.player_one_id) {
                        skipped_first_tank = true;
                        continue;
                    }

                    artillery_state.player_two_id = object.identity.id;
                    break;
                }
            }
        }
    }

    ArtilleryScene::ArtilleryScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void ArtilleryScene::on_enter()
    {
        // Scene activation hook only. Default content is created explicitly via new_scene().
    }

    void ArtilleryScene::on_exit()
    {
        m_state.objects.clear();
        m_artillery_state = {};
    }

    void ArtilleryScene::update_runtime(float dt, const Input& input, bool keyboard_input_enabled)
    {
        m_artillery.update(m_state, m_camera, m_artillery_state, dt, input, keyboard_input_enabled);
    }

    void ArtilleryScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_artillery_state = {};
        m_camera.reset();
        m_grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};
        m_state.editor_commands.clear();
    }

    void ArtilleryScene::restore_defaults()
    {
        reset_runtime_state();

        m_artillery_state.player_one_id = m_state.objects.create_object(
            artillery_factory::create_tank(40.0f, 152.0f, "Player One Tank", "tank-green")
        );

        m_artillery_state.player_two_id = m_state.objects.create_object(
            artillery_factory::create_tank(328.0f, 128.0f, "Player Two Tank", "tank-red")
        );

        if (GameObject* player_two = m_state.objects.get_by_id(m_artillery_state.player_two_id)) {
            player_two->render.sprite.flip_x = true;
        }

        m_artillery.reset_round(m_state, m_artillery_state);

        m_state.objects.select(m_artillery_state.player_one_id);

        m_grid_options.grid_size = 16;
        m_camera.game().x = 0.0f;
        m_camera.game().y = 24.0f;
        m_camera.game().zoom = 2.4f;
        m_camera.game().speed = 256.0f;
        m_camera.state().game_options.follow_target = false;
        m_camera.editor().x = m_camera.game().x;
        m_camera.editor().y = m_camera.game().y;
        m_camera.editor().zoom = m_camera.game().zoom;
        m_camera.activate_game();
    }

    void ArtilleryScene::new_scene()
    {
        restore_defaults();
    }

    ObjectConcept ArtilleryScene::object_concept_for(const GameObject& object) const
    {
        return artillery_concepts::describe_object(object);
    }

    std::span<const SceneCreationAction> ArtilleryScene::scene_creation_actions() const noexcept
    {
        static constexpr std::array<SceneCreationAction, 1> actions{ {
            { "terrain_line", "Terrain Line" }
        } };

        return actions;
    }

    GameObjectId ArtilleryScene::create_scene_object(std::string_view action_id)
    {
        if (action_id == "terrain_line") {
            return add_terrain_line_at_view_center();
        }

        return k_invalid_game_object_id;
    }

    void ArtilleryScene::draw_scene_tools(bool& open)
    {
        tooling::EditorLayout::scene_panel();

        if (ImGui::Begin("Artillery", &open)) {
            draw_creation_tools();
            draw_debug_tools();

            ImGui::Separator();

            m_artillery_tools.draw(m_artillery_state);
        }

        ImGui::End();
    }

    void ArtilleryScene::draw_scene_inspector(GameObject& selected)
    {
        if (!tooling::imgui::layout::collapsing_header("Artillery", true)) {
            return;
        }

        if (!tooling::imgui::property_table::begin("##artillery_inspector")) {
            return;
        }

        const auto kind = artillery_concepts::kind_for(selected);

        if (kind == artillery_concepts::ObjectKind::Tank) {
            tooling::imgui::property_table::text("Current Player", bool_label(selected.identity.id == (m_artillery_state.current_turn == ArtilleryTurn::PlayerOne ? m_artillery_state.player_one_id : m_artillery_state.player_two_id)));
            tooling::imgui::property_table::text("Turn", turn_label(m_artillery_state.current_turn));
            const ArtilleryAim& aim = selected.identity.id == m_artillery_state.player_two_id
                ? m_artillery_state.player_two_aim
                : m_artillery_state.player_one_aim;

            tooling::imgui::property_table::text("Angle", std::to_string(static_cast<int>(aim.angle_degrees)).c_str());
            tooling::imgui::property_table::text("Power", std::to_string(static_cast<int>(aim.power)).c_str());
        }
        else if (kind == artillery_concepts::ObjectKind::TerrainLine) {
            tooling::imgui::property_table::text("Generated Terrain", "Yes");
            tooling::imgui::property_table::text("Projectile Collision", bool_label(selected.collision.solid));
        }
        else if (kind == artillery_concepts::ObjectKind::Projectile) {
            char velocity_buffer[64];
            std::snprintf(velocity_buffer, sizeof(velocity_buffer), "x %.1f, y %.1f", selected.motion.velocity.x, selected.motion.velocity.y);
            tooling::imgui::property_table::text("Velocity", velocity_buffer);
            tooling::imgui::property_table::text("Runtime Object", "Yes");
        }
        else {
            tooling::imgui::property_table::text("Artillery Behaviour", "No Artillery behaviour is assigned");
        }

        tooling::imgui::property_table::end();
    }

    std::string_view ArtilleryScene::default_file_path() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::Artillery)) {
            return descriptor->default_file_path;
        }

        return {};
    }

    std::string_view ArtilleryScene::scene_type_id() const noexcept
    {
        if (const SceneDescriptor* descriptor = scene_descriptor_for(SceneType::Artillery)) {
            return descriptor->id;
        }

        return {};
    }

    void ArtilleryScene::save_scene_data(YAML::Node& root) const
    {
        ArtillerySerializer::save_to_node(m_artillery_state, root);
    }

    bool ArtilleryScene::load_scene_data(const YAML::Node& root, std::string& error)
    {
        ArtilleryState loaded_artillery_state{};

        if (!ArtillerySerializer::load_from_node(root, loaded_artillery_state, error)) {
            return false;
        }

        m_artillery_state = loaded_artillery_state;
        return true;
    }

    bool ArtilleryScene::restore_loaded_scene(SceneState& state, std::string& error)
    {
        restore_loaded_artillery_concepts(state, m_artillery_state);

        if (m_artillery_state.player_one_id == k_invalid_game_object_id ||
            m_artillery_state.player_two_id == k_invalid_game_object_id ||
            m_artillery_state.player_one_id == m_artillery_state.player_two_id) {
            error = "Artillery scene requires two tank objects.";
            return false;
        }

        m_artillery_state.projectile_id = k_invalid_game_object_id;
        m_artillery_state.projectile_owner_id = k_invalid_game_object_id;
        m_artillery_state.projectile_active = false;
        return true;
    }

    void ArtilleryScene::render_overlay(SDL_Renderer* renderer)
    {
        const GameObject* tank = current_tank();
        if (!tank || !tank->lifecycle.active) {
            return;
        }

        const Camera& camera = m_camera.active();
        const float zoom = std::max(camera.zoom, 0.01f);
        const bool firing_right = m_artillery_state.current_turn == ArtilleryTurn::PlayerOne;
        const float direction = firing_right ? 1.0f : -1.0f;
        const ArtilleryAim& aim = current_aim(m_artillery_state);
        const float radians = aim.angle_degrees * std::numbers::pi_v<float> / 180.0f;

        const float start_x = tank->transform.x + (firing_right ? static_cast<float>(tank->size.width) : 0.0f);
        const float start_y = tank->transform.y + 3.0f;
        const float end_x = start_x + std::cos(radians) * direction * 22.0f;
        const float end_y = start_y - std::sin(radians) * 22.0f;

        SDL_SetRenderDrawColor(renderer, 255, 245, 180, 255);
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(std::round((start_x - camera.x) * zoom)),
            static_cast<int>(std::round((start_y - camera.y) * zoom)),
            static_cast<int>(std::round((end_x - camera.x) * zoom)),
            static_cast<int>(std::round((end_y - camera.y) * zoom))
        );
    }

    GameObjectId ArtilleryScene::add_terrain_line_at_view_center()
    {
        GameObject terrain = artillery_factory::create_terrain_line(0.0f, 0.0f, 80, 16, "Terrain Line");
        terrain.transform = first_free_view_center_spawn_position(terrain);

        const GameObjectId id = m_state.objects.create_object(terrain);
        if (GameObject* created = m_state.objects.get_by_id(id)) {
            created->identity.name = "Terrain Line " + std::to_string(id);
        }

        m_state.objects.select(id);
        return id;
    }

    GameObject* ArtilleryScene::current_tank() noexcept
    {
        const GameObjectId id = m_artillery_state.current_turn == ArtilleryTurn::PlayerOne
            ? m_artillery_state.player_one_id
            : m_artillery_state.player_two_id;

        return m_state.objects.get_by_id(id);
    }

    const GameObject* ArtilleryScene::current_tank() const noexcept
    {
        const GameObjectId id = m_artillery_state.current_turn == ArtilleryTurn::PlayerOne
            ? m_artillery_state.player_one_id
            : m_artillery_state.player_two_id;

        return m_state.objects.get_by_id(id);
    }

}
