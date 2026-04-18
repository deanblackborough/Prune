#include "imgui.h"

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/tooling/outliner.hpp"

namespace prune {

    void Outliner::draw(
        GameObjectManager& objects,
        float camera_x,
        float camera_y,
        int viewport_width,
        int viewport_height,
        bool snap_to_grid,
        int grid_size
    ) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

        if (ImGui::Button("Add Block")) {
            const Transform base = next_block_spawn_position(
                static_cast<float>(viewport_width),
                static_cast<float>(viewport_height),
                camera_x,
                camera_y
            );

            const Transform spawn = find_spawn_position(
                objects,
                base.x,
                base.y,
                snap_to_grid,
                grid_size,
                32,
                32
            );

            create_block(objects, spawn.x, spawn.y);
        }

        ImGui::PopStyleColor(3);

        ImGui::Separator();

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##object_search",
            "Search objects...",
            m_object_search.data(),
            m_object_search.size()
        );

        constexpr int visible_rows = 8;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), true)) {
            const std::string_view filter = m_object_search.data();

            for (const auto& object : objects.objects()) {
                if (!filter.empty() && !contains_case_insensitive(object.name, filter)) {
                    continue;
                }

                const bool is_selected = object.id == objects.selected_id();
                const std::string label = object_label(object);

                if (ImGui::Selectable(label.c_str(), is_selected)) {
                    objects.select(object.id);
                }
            }
        }
        ImGui::EndChild();
    }

    std::string Outliner::object_label(const GameObject& object)
    {
        const char* type = object.is_player ? "[Player]" : "[Block]";
        return std::string(type) + " " + object.name;
    }

    GameObjectId Outliner::create_block(GameObjectManager& objects, float x, float y)
    {
        GameObject block;
        block.transform.x = x;
        block.transform.y = y;
        block.rectangle.width = 32;
        block.rectangle.height = 32;
        block.rectangle.color[0] = random_color_component();
        block.rectangle.color[1] = random_color_component();
        block.rectangle.color[2] = random_color_component();
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        const GameObjectId id = objects.create_object(block);

        if (GameObject* created = objects.get_by_id(id)) {
            created->name = "Block " + std::to_string(id);
        }

        objects.select(id);

        return id;
    }

    float Outliner::random_color_component()
    {
        return m_color_dist(m_rng);
    }

    Transform Outliner::next_block_spawn_position(
        float viewport_width,
        float viewport_height,
        float camera_x,
        float camera_y
    ) const
    {
        return Transform{
            camera_x + (viewport_width * 0.5f),
            camera_y + (viewport_height * 0.5f)
        };
    }

    Transform Outliner::find_spawn_position(
        const GameObjectManager& objects,
        float base_x,
        float base_y,
        bool snap_to_grid,
        int grid_size,
        int object_width,
        int object_height
    ) const
    {
        const float step = snap_to_grid
            ? static_cast<float>(std::max(1, grid_size))
            : 32.0f;

        float start_x = base_x;
        float start_y = base_y;

        if (snap_to_grid) {
            start_x = std::round(start_x / step) * step;
            start_y = std::round(start_y / step) * step;
        }

        if (is_space_free(objects, start_x, start_y, object_width, object_height)) {
            return Transform{ start_x, start_y };
        }

        for (int ring = 1; ring <= 8; ++ring) {
            for (int dy = -ring; dy <= ring; ++dy) {
                for (int dx = -ring; dx <= ring; ++dx) {
                    if (std::abs(dx) != ring && std::abs(dy) != ring) {
                        continue;
                    }

                    const float candidate_x = start_x + (static_cast<float>(dx) * step);
                    const float candidate_y = start_y + (static_cast<float>(dy) * step);

                    if (is_space_free(objects, candidate_x, candidate_y, object_width, object_height)) {
                        return Transform{ candidate_x, candidate_y };
                    }
                }
            }
        }

        return Transform{ start_x, start_y };
    }

    bool Outliner::is_space_free(
        const GameObjectManager& objects,
        float x,
        float y,
        int width,
        int height
    ) const
    {
        const float left_a = x;
        const float right_a = x + static_cast<float>(width);
        const float top_a = y;
        const float bottom_a = y + static_cast<float>(height);

        for (const auto& object : objects.objects()) {
            if (!object.active) {
                continue;
            }

            const float left_b = object.transform.x;
            const float right_b = object.transform.x + static_cast<float>(object.rectangle.width);
            const float top_b = object.transform.y;
            const float bottom_b = object.transform.y + static_cast<float>(object.rectangle.height);

            const bool overlaps =
                left_a < right_b &&
                right_a > left_b &&
                top_a < bottom_b &&
                bottom_a > top_b;

            if (overlaps) {
                return false;
            }
        }

        return true;
    }

    bool Outliner::contains_case_insensitive(std::string_view text, std::string_view query) const
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
