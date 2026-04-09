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
        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##object_search",
            "Search objects...",
            m_editor_state.object_search.data(),
            m_editor_state.object_search.size()
        );

        constexpr int visible_rows = 5;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), true)) {
            const std::string_view filter = m_editor_state.object_search.data();

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