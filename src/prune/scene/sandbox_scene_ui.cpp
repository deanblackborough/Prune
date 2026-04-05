#include "sandbox_scene.hpp"

#include <algorithm>
#include <string_view>

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
    }

    void SandboxScene::draw_selected_object_ui()
    {
        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            return;
        }

        const bool is_player = (selected->id == m_player_id);

        ImGui::Separator();

        ImGui::TextUnformatted("Selected");
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
                    created->name = make_unique_name(source_name, clone_id);
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
            bool player_flag = is_player;
            ImGui::BeginDisabled();
            ImGui::Checkbox("IsPlayer", &player_flag);
            ImGui::EndDisabled();
        }

        selected->clamp_to_area(m_window_width, m_window_height);
    }

} // namespace prune