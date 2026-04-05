#include "sandbox_scene.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <cmath>
#include "imgui.h"
#include "prune/core/input.hpp"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    void SandboxScene::on_enter()
    {
        m_objects.clear();

        GameObject player;
        player.name = "Player";
        player.transform.x = 100.0f;
        player.transform.y = 100.0f;
        player.rectangle.width = 50;
        player.rectangle.height = 50;
        player.rectangle.color[0] = 0.3f;
        player.rectangle.color[1] = 0.8f;
        player.rectangle.color[2] = 0.5f;
        player.visible = true;
        player.solid = false;
        player.is_player = true;

        m_player_id = m_objects.create_object(player);

        GameObject block;
        block.name = "Static Block";
        block.transform.x = 420.0f;
        block.transform.y = 220.0f;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        m_objects.create_object(block);

        m_objects.select(m_player_id);
    }

    GameObject* SandboxScene::pick_object_at(int x, int y) noexcept
    {
        auto& objects = m_objects.objects();

        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            GameObject& object = *it;

            if (!object.active || !object.visible) {
                continue;
            }

            const RectF bounds = object.bounds();

            const bool inside =
                static_cast<float>(x) >= bounds.x &&
                static_cast<float>(x) <= (bounds.x + bounds.width) &&
                static_cast<float>(y) >= bounds.y &&
                static_cast<float>(y) <= (bounds.y + bounds.height);

            if (inside) {
                return &object;
            }
        }

        return nullptr;
    }

    void SandboxScene::handle_scene_click(const Input& input)
    {
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }

        if (!input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            return;
        }

        GameObject* clicked = pick_object_at(input.mouse_x(), input.mouse_y());
        if (!clicked) {
            return;
        }

        m_objects.select(clicked->id);
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        handle_scene_click(input);

        GameObject* player = player_object();
        if (!player) {
            return;
        }

        m_player_controller.update(*player, dt, input);
        resolve_player_collisions(*player);
        player->clamp_to_area(m_window_width, m_window_height);
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        const GameObjectId selected_id = m_objects.selected_id();
        SDL_Rect selected_outline{};
        bool has_selected_outline = false;

        for (const auto& object : m_objects.objects()) {
            if (!object.active || !object.visible) {
                continue;
            }

            SDL_Rect rect{
                static_cast<int>(object.transform.x),
                static_cast<int>(object.transform.y),
                object.rectangle.width,
                object.rectangle.height
            };

            SDL_SetRenderDrawColor(
                renderer,
                static_cast<Uint8>(object.rectangle.color[0] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[1] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[2] * 255.0f),
                255
            );

            SDL_RenderFillRect(renderer, &rect);

            if (m_highlight_selected && object.id == selected_id) {
                selected_outline = SDL_Rect{
                    rect.x - 2,
                    rect.y - 2,
                    rect.w + 4,
                    rect.h + 4
                };

                has_selected_outline = true;
            }
        }

        if (m_highlight_selected && has_selected_outline) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderDrawRect(renderer, &selected_outline);
        }
    }

    void SandboxScene::draw_inspector_ui()
    {
        if (ImGui::Button("Add Block")) {
            const Transform spawn = next_block_spawn_position();
            create_block(spawn.x, spawn.y);
        }

        ImGui::Checkbox("Highlight selected", &m_highlight_selected);

        ImGui::Separator();

        draw_object_list_ui();

        ImGui::Spacing();

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        draw_selected_object_ui();
    }

    void SandboxScene::draw_debug_ui()
    {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Object count: %d", static_cast<int>(m_objects.count()));
        ImGui::Text("Selected id: %u", m_objects.selected_id());
        ImGui::Text("Player id: %u", m_player_id);

        if (const GameObject* player = player_object()) {
            ImGui::Separator();
            ImGui::TextUnformatted("Player");
            ImGui::Text("Position: %.1f, %.1f", player->transform.x, player->transform.y);
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

    std::string SandboxScene::make_unique_name(std::string desired, GameObjectId ignore_id) const
    {
        if (desired.empty()) {
            desired = "Object";
        }

        auto is_taken = [&](const std::string& name) {
            for (const auto& obj : m_objects.objects()) {
                if (obj.id != ignore_id && obj.name == name) {
                    return true;
                }
            }
            return false;
        };

        if (!is_taken(desired)) {
            return desired;
        }

        int suffix = 1;
        std::string candidate;

        do {
            candidate = desired + " " + std::to_string(suffix++);
        } while (is_taken(candidate));

        return candidate;
    }

    Transform SandboxScene::next_block_spawn_position() const noexcept
    {
        constexpr float base_x = 160.0f;
        constexpr float base_y = 160.0f;
        constexpr float offset_step = 50.0f;
        constexpr int block_size = 50;

        const float offset = static_cast<float>(m_objects.count()) * offset_step;

        Transform spawn{
            base_x + offset,
            base_y + offset
        };

        const float max_x = std::max(0.0f, static_cast<float>(m_window_width - block_size));
        const float max_y = std::max(0.0f, static_cast<float>(m_window_height - block_size));

        spawn.x = std::clamp(spawn.x, 0.0f, max_x);
        spawn.y = std::clamp(spawn.y, 0.0f, max_y);

        return spawn;
    }

    GameObjectId SandboxScene::create_block(float x, float y)
    {
        GameObject block;
        block.name = "Block";
        block.transform.x = x;
        block.transform.y = y;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        const GameObjectId id = m_objects.create_object(block);

        if (GameObject* created = m_objects.get_by_id(id)) {
            created->name = "Block " + std::to_string(id);
            created->clamp_to_area(m_window_width, m_window_height);
        }

        m_objects.select(id);

        return id;
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

    namespace { // Anonymous namespace
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

    void SandboxScene::draw_object_list_ui()
    {
        ImGui::TextUnformatted("Objects");
        ImGui::Separator();

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint("##object_search", "Search objects...", m_object_search.data(), m_object_search.size());

        constexpr int visible_rows = 5;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), true)) {
            const std::string_view filter = m_object_search.data();

            for (const auto& object : m_objects.objects()) {
                if (!filter.empty()) {
                    if (!contains_case_insensitive(object.name, filter)) {
                        continue;
                    }
                }

                const bool is_selected = object.id == m_objects.selected_id();
                if (ImGui::Selectable(object.name.c_str(), is_selected)) {
                    m_objects.select(object.id);
                }
            }
        }
        ImGui::EndChild();
    }

    void SandboxScene::draw_selected_object_ui()
    {
        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            return;
        }

        bool is_player = (selected->id == m_player_id);

        ImGui::Separator();

        ImGui::TextUnformatted("Selected");
        ImGui::Separator();

        ImGui::Text("Id: %u", selected->id);

        if (is_player) {
            ImGui::Text("Name: %s", selected->name.c_str());
        } else {
            char name_buffer[128]{};
            std::snprintf(name_buffer, sizeof(name_buffer), "%s", selected->name.c_str());

            if (ImGui::InputText("Name", name_buffer, sizeof(name_buffer))) {
                selected->name = make_unique_name(name_buffer, selected->id);
            }
        }

        if (!is_player) {
            ImGui::Separator();

            if (ImGui::Button("Delete")) {
                const GameObjectId id_to_remove = selected->id;
                m_objects.remove_object(id_to_remove);
                return;
            }

            ImGui::SameLine();

            if (ImGui::Button("Clone")) {
                const std::string source_name = selected->name;

                GameObject clone = *selected;
                clone.is_player = false;
                clone.transform.x += 50.0f;
                clone.transform.y += 50.0f;

                const GameObjectId clone_id = m_objects.create_object(clone);

                if (GameObject* created = m_objects.get_by_id(clone_id)) {
                    created->name = source_name + " Copy";
                    created->clamp_to_area(m_window_width, m_window_height);
                }

                m_objects.select(clone_id);
                return;
            }
        }

        ImGui::Separator();

        ImGui::TextUnformatted("Position");

        const auto max_x = static_cast<float>(m_window_width - selected->rectangle.width);
        const auto max_y = static_cast<float>(m_window_height - selected->rectangle.height);

        ImGui::SliderFloat("X", &selected->transform.x, 0.0f, std::max(0.0f, max_x));
        ImGui::SliderFloat("Y", &selected->transform.y, 0.0f, std::max(0.0f, max_y));

        ImGui::Separator();

        ImGui::TextUnformatted("Properties");
        ImGui::SliderInt("Width", &selected->rectangle.width, 10, 200);
        ImGui::SliderInt("Height", &selected->rectangle.height, 10, 200);
        ImGui::ColorEdit3("Colour", selected->rectangle.color);

        if (is_player) {
            float speed = m_player_controller.speed();
            if (ImGui::SliderFloat("Move Speed", &speed, 50.0f, 600.0f, "%.1f")) {
                m_player_controller.set_speed(speed);
            }
        }

        ImGui::Separator();

        ImGui::TextUnformatted("Flags");
        ImGui::Checkbox("Active", &selected->active);
        ImGui::Checkbox("Visible", &selected->visible);
        if (is_player) {
            ImGui::BeginDisabled();
            ImGui::Checkbox("Solid", &selected->solid);
            ImGui::EndDisabled();

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
                ImGui::SetTooltip("Player solid value not used yet, player checks against game objects only");
            }
        } else {
            ImGui::Checkbox("Solid", &selected->solid);
        }

        if (is_player) {
            ImGui::BeginDisabled();
            ImGui::Checkbox("IsPlayer", &is_player);
            ImGui::EndDisabled();
        }

        selected->clamp_to_area(m_window_width, m_window_height);
    }

} // namespace prune