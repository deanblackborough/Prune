#include <cctype>

#include "imgui.h"

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/outliner.hpp"

namespace prune {

    void Outliner::draw(GameObjectManager& objects, const Scene& scene) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.2f, 0.6f, 1.0f));        // Normal state
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.3f, 0.7f, 1.0f)); // Hover state
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.1f, 0.5f, 1.0f));

        ImGui::PopStyleColor(3);

        ImGui::Separator();

        ImGui::SetNextItemWidth(-1.0f);
        ImGui::InputTextWithHint(
            "##object_search",
            "Search objects...",
            m_object_search.data(),
            m_object_search.size()
        );

        constexpr int visible_rows = 5;
        const float row_height = ImGui::GetTextLineHeightWithSpacing();
        const float list_height = row_height * static_cast<float>(visible_rows)
            + ImGui::GetStyle().FramePadding.y * 2.0f;

        if (ImGui::BeginChild("object_list", ImVec2(0.0f, list_height), ImGuiChildFlags_Borders)) {
            const std::string_view filter = m_object_search.data();

            for (const auto& object : objects.objects()) {
                const std::string label = object_label(object, scene);
                const ObjectConcept object_concept = scene.object_concept_for(object);

                if (!filter.empty() &&
                    !contains_case_insensitive(label, filter) &&
                    !contains_case_insensitive(object.identity.name, filter)) {
                    continue;
                }

                const bool is_selected = object.identity.id == objects.selected_id();

                if (!object_concept.selectable || !object.editor.selectable) {
                    ImGui::BeginDisabled();
                    ImGui::Selectable(label.c_str(), is_selected);
                    ImGui::EndDisabled();
                    continue;
                }

                if (ImGui::Selectable(label.c_str(), is_selected)) {
                    objects.select(object.identity.id);
                }
            }
        }
        ImGui::EndChild();
    }

    std::string Outliner::object_label(const GameObject& object, const Scene& scene)
    {
        std::string label = "[";
        const ObjectConcept object_concept = scene.object_concept_for(object);

        label += object_concept.label;
        label += "] ";
        label += object.identity.name;

        if (object_concept.runtime_only) {
            label += " (runtime)";
        }

        if (!object.lifecycle.active) {
            label += " (inactive)";
        }

        return label;
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
