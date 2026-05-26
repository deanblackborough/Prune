#include "prune/scene/world_scene.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>
#include <utility>

#include "imgui.h"

#include "prune/scene/collision.hpp"
#include "prune/tooling/imgui/layout.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/scene/scene_serializer.hpp"

namespace prune {

    void WorldScene::update(float dt, const Input& input)
    {
        update_runtime(dt, input, scene_keyboard_input_enabled());
        m_camera.update_game_camera(m_state.viewport, game_camera_target());
    }

    void WorldScene::update_editor(float dt, const Input& input)
    {
        m_interaction.update(*this, m_state, m_camera, m_grid_options, dt, input);
        m_camera.update_game_camera(m_state.viewport, game_camera_target());
    }

    void WorldScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, *this, m_state, m_camera, m_grid_options);
        render_overlay(renderer);
    }

    void WorldScene::draw_viewport_overlays()
    {
        if (!m_state.viewport.has_area()) {
            return;
        }

        if (!m_state.scene_options.debug_overlays.show_role_labels) {
            return;
        }

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const Camera& camera = m_camera.active();
        const float zoom = std::max(camera.zoom, 0.01f);
        const ImU32 label_colour = IM_COL32(230, 220, 245, 230);
        const ImU32 label_shadow_colour = IM_COL32(18, 14, 24, 220);

        for (const auto& object : m_state.objects.objects()) {
            if (!object.lifecycle.active || !object.render.visible) {
                continue;
            }

            const std::string label = object_role_label(object);
            if (label.empty()) {
                continue;
            }

            const float screen_x =
                static_cast<float>(m_state.viewport.screen_x) +
                ((object.transform.x - camera.x) * zoom);

            const float screen_y =
                static_cast<float>(m_state.viewport.screen_y) +
                ((object.transform.y - camera.y) * zoom) - 18.0f;

            const ImVec2 pos{ screen_x, screen_y };
            draw_list->AddText(ImVec2(pos.x + 1.0f, pos.y + 1.0f), label_shadow_colour, label.c_str());
            draw_list->AddText(pos, label_colour, label.c_str());
        }
    }

    void WorldScene::draw_creation_tools()
    {
        if (!tooling::imgui::layout::collapsing_header("Game Tools", true)) {
            return;
        }

        tooling::imgui::layout::spacing(3);

        const std::span<const SceneCreationAction> actions = scene_creation_actions();
        if (actions.empty()) {
            ImGui::TextUnformatted("No creation actions for this scene.");
            return;
        }

        bool first = true;
        for (const SceneCreationAction& action : actions) {
            if (!first) {
                ImGui::SameLine();
            }

            ImGui::PushID(action.id.data());
            if (ImGui::Button(action.label.data())) {
                create_scene_object(action.id);
            }
            ImGui::PopID();

            first = false;
        }

        tooling::imgui::layout::spacing(3);
    }

    void WorldScene::draw_debug_tools()
    {
        if (!tooling::imgui::layout::collapsing_header("Debug")) {
            return;
        }

        DebugOverlayOptions& overlays = m_state.scene_options.debug_overlays;

        if (tooling::imgui::property_table::begin("Debug")) {
            tooling::imgui::property_table::checkbox("Collision bounds", "###collision_bounds", overlays.show_collision_bounds);
            tooling::imgui::property_table::checkbox("Runtime object markers", "###runtime_markers", overlays.show_runtime_markers);
            tooling::imgui::property_table::checkbox("Scene role labels", "###scene_role_labels", overlays.show_role_labels);
            tooling::imgui::property_table::end();
        }
    }

    bool WorldScene::save_to_file(std::string_view path, std::string& error) const
    {
        try {
            YAML::Node root;
            root["scene_type"] = std::string(scene_type_id());

            SceneSerializer::save_to_node(m_state, m_camera, m_grid_options, root);
            save_scene_data(root);

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

    bool WorldScene::load_from_file(std::string_view path, std::string& error)
    {
        try {
            const YAML::Node root = YAML::LoadFile(std::string(path));

            if (!root["scene_type"] || root["scene_type"].as<std::string>() != scene_type_id()) {
                error = "Save file is not a " + std::string(scene_name()) + " scene.";
                return false;
            }

            SceneState loaded_state = m_state;
            SceneCamera loaded_camera = m_camera;
            GridOptions loaded_grid_options = m_grid_options;

            if (!SceneSerializer::load_from_node(loaded_state, loaded_camera, loaded_grid_options, root, error)) {
                return false;
            }

            if (!load_scene_data(root, error)) {
                return false;
            }

            if (!restore_loaded_scene(loaded_state, error)) {
                return false;
            }

            m_state = std::move(loaded_state);
            m_camera = loaded_camera;
            m_grid_options = loaded_grid_options;
            m_camera.update_game_camera(m_state.viewport, game_camera_target());

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

    void WorldScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    GameObjectManager& WorldScene::get_object_manager()
    {
        return m_state.objects;
    }

    SceneOptions& WorldScene::get_scene_options()
    {
        return m_state.scene_options;
    }

    WorldSceneContext WorldScene::world_scene_context() noexcept
    {
        return WorldSceneContext{ &m_grid_options, &m_camera };
    }

    ConstWorldSceneContext WorldScene::world_scene_context() const noexcept
    {
        return ConstWorldSceneContext{ &m_grid_options, &m_camera };
    }

    bool WorldScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.keyboard_input_enabled && m_state.viewport.has_area();
    }

    bool WorldScene::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }

    Transform WorldScene::view_center_spawn_position(int width, int height) const
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

        const float grid = static_cast<float>(m_grid_options.grid_size);
        transform.x = std::floor(transform.x / grid) * grid;
        transform.y = std::floor(transform.y / grid) * grid;
        return transform;
    }

    Transform WorldScene::first_free_view_center_spawn_position(const GameObject& object) const
    {
        const int grid_size = m_grid_options.grid_size > 0
            ? m_grid_options.grid_size
            : object.size.width;

        const Transform base = view_center_spawn_position(object.size.width, object.size.height);

        constexpr int max_radius = 20;

        for (int radius = 0; radius <= max_radius; ++radius) {
            for (int y = -radius; y <= radius; ++y) {
                for (int x = -radius; x <= radius; ++x) {
                    if (std::abs(x) != radius && std::abs(y) != radius) {
                        continue;
                    }

                    GameObject candidate = object;
                    candidate.transform.x = base.x + static_cast<float>(x * grid_size);
                    candidate.transform.y = base.y + static_cast<float>(y * grid_size);

                    if (is_space_free(candidate)) {
                        return candidate.transform;
                    }
                }
            }
        }

        return base;
    }

    bool WorldScene::is_space_free(const GameObject& candidate) const noexcept
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
