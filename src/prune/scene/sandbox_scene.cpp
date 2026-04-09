#include "sandbox_scene.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>

#include "imgui.h"

namespace prune {

    namespace {
        bool contains_case_insensitive(std::string_view text, std::string_view query)
        {
            if (query.empty()) {
                return true;
            }

            auto to_lower = [](unsigned char c) {
                return static_cast<char>(std::tolower(c));
            };

            for (std::size_t i = 0; i + query.size() <= text.size(); ++i) {
                bool matches = true;

                for (std::size_t j = 0; j < query.size(); ++j) {
                    if (to_lower(static_cast<unsigned char>(text[i + j])) !=
                        to_lower(static_cast<unsigned char>(query[j]))) {
                        matches = false;
                        break;
                        }
                }

                if (matches) {
                    return true;
                }
            }

            return false;
        }
    }

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::on_enter()
    {
        m_objects.clear();

        m_player_id = m_objects.create_object(create_player());
        m_objects.create_object(create_initial_block());

        m_objects.select(m_player_id);
    }

    GameObject SandboxScene::create_player() {
        GameObject player;
        player.name = "Player";
        player.transform.x = 128.0f;
        player.transform.y = 128.0f;
        player.rectangle.width = 32;
        player.rectangle.height = 32;
        player.rectangle.color[0] = 0.3f;
        player.rectangle.color[1] = 0.8f;
        player.rectangle.color[2] = 0.5f;
        player.active = true;
        player.visible = true;
        player.solid = false;
        player.is_player = true;

        return player;
    }

    GameObject SandboxScene::create_initial_block() {
        GameObject block;
        block.name = "Static Block";
        block.transform.x = 128.0f;
        block.transform.y = 256.0f;
        block.rectangle.width = 32;
        block.rectangle.height = 32;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        return block;
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        update_editor(dt, input);
        update_game(dt, input);
    }

    void SandboxScene::update_game(float dt, const Input& input)
    {
        update_player(dt, input);
    }

    void SandboxScene::update_player(float dt, const Input& input)
    {
        GameObject* player = player_object();
        if (!player) {
            return;
        }

        player->velocity = m_player_controller.movement_velocity(input);
        move_object(*player, player->velocity.x * dt, player->velocity.y * dt, true);
    }

    void SandboxScene::move_object(GameObject& object, float delta_x, float delta_y, bool resolve_collisions)
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_collisions && object.is_player) {
            resolve_player_collisions(object);
        }
    }

    Transform SandboxScene::screen_to_world(int screen_x, int screen_y) const noexcept
    {
        return {
            static_cast<float>(screen_x) + camera_x,
            static_cast<float>(screen_y) + camera_y
        };
    }

    SDL_Rect SandboxScene::world_to_screen_rect(const GameObject& object) const noexcept
    {
        return SDL_Rect{
            static_cast<int>(std::round(object.transform.x - camera_x)),
            static_cast<int>(std::round(object.transform.y - camera_y)),
            object.rectangle.width,
            object.rectangle.height
        };
    }

    bool SandboxScene::is_rect_visible(const SDL_Rect& rect) const noexcept
    {
        return rect.x + rect.w >= 0 &&
               rect.y + rect.h >= 0 &&
               rect.x < m_window_width &&
               rect.y < m_window_height;
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        draw_grid(renderer);

        const GameObjectId selected_id = m_objects.selected_id();
        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : m_objects.objects()) {
            if (!object.active || !object.visible) {
                continue;
            }

            const SDL_Rect rect = world_to_screen_rect(object);

            if (!is_rect_visible(rect)) {
                continue;
            }

            SDL_SetRenderDrawColor(
                renderer,
                static_cast<Uint8>(object.rectangle.color[0] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[1] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[2] * 255.0f),
                255
            );

            SDL_RenderFillRect(renderer, &rect);

            if (highlight_selected && object.id == selected_id) {
                selected_outline = SDL_Rect{
                    rect.x - 2,
                    rect.y - 2,
                    rect.w + 4,
                    rect.h + 4
                };

                has_selected_outline = true;
            }
        }

        if (highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SandboxScene::draw_grid(SDL_Renderer* renderer) const
    {
        if (!show_grid || grid_size <= 1) {
            return;
        }

        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);

        for (int x = 0; x < m_window_width; x += grid_size) {
            SDL_RenderDrawLine(renderer, x, 0, x, m_window_height);
        }

        for (int y = 0; y < m_window_height; y += grid_size) {
            SDL_RenderDrawLine(renderer, 0, y, m_window_width, y);
        }
    }

    float SandboxScene::snap_value_to_grid(float value) const noexcept
    {
        const int scene_grid_size = std::max(1, grid_size);
        return std::round(value / static_cast<float>(scene_grid_size)) * static_cast<float>(scene_grid_size);
    }

    void SandboxScene::snap_object_to_grid(GameObject& object) const noexcept
    {
        object.transform.x = snap_value_to_grid(object.transform.x);
        object.transform.y = snap_value_to_grid(object.transform.y);
    }

    void SandboxScene::draw_viewport_panel()
    {
        if (ImGui::CollapsingHeader("Grid", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Show", &show_grid);
            ImGui::Checkbox("Snap to grid", &snap_to_grid);
            ImGui::SliderInt("Grid size", &grid_size, min_grid_size, max_grid_size);
            ImGui::SliderInt("Nudge step", &nudge_step, min_nudge_step, max_nudge_step);
        }

        if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::SliderFloat("Camera X", &camera_x, -4096.0f, 4096.0f);
            ImGui::SliderFloat("Camera Y", &camera_y, -4096.0f, 4096.0f);
            ImGui::SliderFloat("Speed", &camera_speed, 64.0f, 512.0f);
        }
    }

    void SandboxScene::draw_outline_panel()
    {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

        if (ImGui::Button("Add Block")) {
            const Transform spawn = next_block_spawn_position();
            create_block(spawn.x, spawn.y);
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::Checkbox("Highlight selected", &highlight_selected);

        if (ImGui::CollapsingHeader("Objects", ImGuiTreeNodeFlags_DefaultOpen)) {
            draw_object_list_ui();
        }
    }

    void SandboxScene::draw_object_panel()
    {
        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        if (ImGui::CollapsingHeader("Selected", ImGuiTreeNodeFlags_DefaultOpen)) {

            const bool is_player = (selected->id == m_player_id);

            if (is_player) {
                float speed = m_player_controller.speed();
                if (ImGui::SliderFloat("Speed", &speed, 32.0f, 512.0f)) {
                    m_player_controller.set_speed(speed);
                }
            }

            ImGui::DragFloat("Object X", &selected->transform.x, 1.0f);
            ImGui::DragFloat("Object Y", &selected->transform.y, 1.0f);

            if (snap_to_grid && !is_player) {
                snap_object_to_grid(*selected);
            }

            const Transform screen_pos = {
                selected->transform.x - camera_x,
                selected->transform.y - camera_y
            };

            ImGui::Text("Screen Position: %.1f, %.1f", screen_pos.x, screen_pos.y);

            if (ImGui::CollapsingHeader("Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("Width", &selected->rectangle.width, 16, 256);
                ImGui::SliderInt("Height", &selected->rectangle.height, 16, 256);
                ImGui::ColorEdit3("Colour", selected->rectangle.color);
            }

            if (ImGui::CollapsingHeader("Flags")) {
                ImGui::Checkbox("Active", &selected->active);
                ImGui::Checkbox("Visible", &selected->visible);

                if (is_player) {
                    ImGui::BeginDisabled();
                    ImGui::Checkbox("Solid", &selected->solid);
                    ImGui::EndDisabled();

                    if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                        ImGui::SetTooltip("Player solid value not used yet, player checks against game objects only");
                    }

                    bool player_flag = true;
                    ImGui::BeginDisabled();
                    ImGui::Checkbox("IsPlayer", &player_flag);
                    ImGui::EndDisabled();
                } else {
                    ImGui::Checkbox("Solid", &selected->solid);
                }
            }
        }
    }

    void SandboxScene::draw_debug_ui()
    {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Object count: %d", static_cast<int>(m_objects.count()));
        ImGui::Text("Selected id: %u", m_objects.selected_id());
        ImGui::Text("Player id: %u", m_player_id);
        ImGui::Text("Grid: %s", show_grid ? "On" : "Off");
        ImGui::Text("Snap: %s", snap_to_grid ? "On" : "Off");
        ImGui::Text("Grid size: %d", grid_size);
        ImGui::Text("Camera: %.1f, %.1f", camera_x, camera_y);
        ImGui::Text("Camera speed: %.1f", camera_speed);

        if (const GameObject* player = player_object()) {
            ImGui::Separator();
            ImGui::TextUnformatted("Player");
            ImGui::Text("World Position: %.1f, %.1f", player->transform.x, player->transform.y);
            ImGui::Text("Velocity: %.1f, %.1f", player->velocity.x, player->velocity.y);
            ImGui::Text("Speed: %.1f", m_player_controller.speed());
        }
    }

    GameObject* SandboxScene::player_object() noexcept
    {
        return m_objects.get_by_id(m_player_id);
    }

    const GameObject* SandboxScene::player_object() const noexcept
    {
        return m_objects.get_by_id(m_player_id);
    }

    bool SandboxScene::is_overlapping(const GameObject& a, const GameObject& b) const noexcept
    {
        const RectF a_bounds = a.bounds();
        const RectF b_bounds = b.bounds();

        return a_bounds.x < (b_bounds.x + b_bounds.width) &&
               (a_bounds.x + a_bounds.width) > b_bounds.x &&
               a_bounds.y < (b_bounds.y + b_bounds.height) &&
               (a_bounds.y + a_bounds.height) > b_bounds.y;
    }

    Transform SandboxScene::next_block_spawn_position() const noexcept
    {
        constexpr float offset_step = 32.0f;

        const float offset = static_cast<float>(m_objects.count()) * offset_step;

        return Transform{
            camera_x + offset,
            camera_y + offset
        };
    }

    void SandboxScene::resolve_player_collisions(GameObject& player)
    {
        for (const auto& object : m_objects.objects()) {
            if (object.id == player.id || !object.active || !object.solid) {
                continue;
            }

            if (!is_overlapping(player, object)) {
                continue;
            }

            const RectF player_bounds = player.bounds();
            const RectF block_bounds = object.bounds();

            const float overlap_left = (player_bounds.x + player_bounds.width) - block_bounds.x;
            const float overlap_right = (block_bounds.x + block_bounds.width) - player_bounds.x;
            const float overlap_top = (player_bounds.y + player_bounds.height) - block_bounds.y;
            const float overlap_bottom = (block_bounds.y + block_bounds.height) - player_bounds.y;

            const float resolve_x = (overlap_left < overlap_right) ? -overlap_left : overlap_right;
            const float resolve_y = (overlap_top < overlap_bottom) ? -overlap_top : overlap_bottom;

            if (std::abs(resolve_x) < std::abs(resolve_y)) {
                player.transform.x += resolve_x;
                player.velocity.x = 0.0f;
            } else {
                player.transform.y += resolve_y;
                player.velocity.y = 0.0f;
            }
        }
    }

    void SandboxScene::draw_object_list_ui()
    {
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##object_search",
            "Search objects...",
            object_search.data(),
            object_search.size()
        );

        constexpr int visible_rows = 5;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), true)) {
            const std::string_view filter = object_search.data();

            for (const auto& object : m_objects.objects()) {
                if (!filter.empty() && !contains_case_insensitive(object.name, filter)) {
                    continue;
                }

                const bool is_selected = object.id == m_objects.selected_id();
                if (ImGui::Selectable(object.name.c_str(), is_selected)) {
                    m_objects.select(object.id);
                }
            }
        }
        ImGui::EndChild();

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            return;
        }

        const bool is_player = (selected->id == m_player_id);

        ImGui::Separator();

        ImGui::Text("Id: %u", selected->id);

        if (is_player) {
            ImGui::Text("Name: %s", selected->name.c_str());
        } else {
            char name_buffer[128]{};
            std::snprintf(name_buffer, sizeof(name_buffer), "%s", selected->name.c_str());

            ImGui::InputText("Name", name_buffer, sizeof(name_buffer));
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                selected->name = make_unique_name(name_buffer, selected->id);
            }
        }

        if (!is_player) {
            if (ImGui::Button("Delete")) {
                const GameObjectId id_to_remove = selected->id;
                m_objects.remove_object(id_to_remove);
                return;
            }

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

            if (ImGui::Button("Clone")) {
                const std::string source_name = selected->name;

                GameObject clone = *selected;
                clone.is_player = false;
                clone.transform.x += 32.0f;
                clone.transform.y += 32.0f;

                const GameObjectId clone_id = m_objects.create_object(clone);

                if (GameObject* created = m_objects.get_by_id(clone_id)) {
                    created->name = make_unique_name(source_name, clone_id);
                }

                m_objects.select(clone_id);

                ImGui::PopStyleColor(3);
                return;
            }
            ImGui::PopStyleColor(3);
        }
    }
} // namespace prune