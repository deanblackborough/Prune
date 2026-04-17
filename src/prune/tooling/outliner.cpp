#include "imgui.h"

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/tooling/outliner.hpp"

#include <array>

namespace prune {

    void Outliner::draw(GameObjectManager& objects, float camera_x, float camera_y) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

        if (ImGui::Button("Add Object (Temp)")) {
            const Transform spawn = next_block_spawn_position(
                objects,
                ImGui::GetWindowSize().x,
                ImGui::GetWindowWidth(),
                camera_x,
                camera_y
            );
            create_block(objects, spawn.x, spawn.y);
        }

        ImGui::PopStyleColor(3);

        ImGui::Separator();

        std::array<char, 128> object_search{};

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##object_search",
            "Search objects...",
            object_search.data(),
            object_search.size()
        );

        constexpr int visible_rows = 8;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), true)) {
            const std::string_view filter = object_search.data();

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
        GameObjectManager& objects,
        float width_height,
        float window_width,
        float camera_x,
        float camera_y
    ) const
    {
        constexpr float offset_step = 32.0f;

        const float offset = static_cast<float>(objects.count()) * offset_step;

        return Transform{
            camera_x + (width_height * 0.5f) + offset,
            camera_y + (window_width * 0.5f) + offset
        };
    }

    bool Outliner::contains_case_insensitive(std::string_view text, std::string_view query)
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
