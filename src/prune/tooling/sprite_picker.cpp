#include "prune/tooling/sprite_picker.hpp"

#include <string_view>

#include "imgui.h"

#include "prune/resources/sprites.hpp"
#include "prune/tooling/imgui/property_table.hpp"

namespace prune::tooling {

    namespace {

        const char* sprite_combo_preview(const std::string& selected_sprite_key)
        {
            if (const Sprites* resource = find_sprite_resource(selected_sprite_key)) {
                return resource->key.data();
            }

            if (!selected_sprite_key.empty()) {
                return selected_sprite_key.c_str();
            }

            return "No sprite selected";
        }

    }

    bool draw_sprite_picker(
        const char* label,
        const char* id,
        std::string& selected_sprite_key
    ) {
        const auto& resources = sprite_resources();

        if (resources.empty()) {
            imgui::property_table::text(label, "No sprites defined");
            return false;
        }

        bool changed = false;

        imgui::property_table::begin_row(label);

        ImGui::PushID(id);

        if (ImGui::BeginCombo("##sprite", sprite_combo_preview(selected_sprite_key))) {
            std::string_view current_category;

            for (const Sprites& resource : resources) {
                if (resource.category != current_category) {
                    if (!current_category.empty()) {
                        ImGui::Separator();
                    }

                    current_category = resource.category;
                    ImGui::TextDisabled("%s", resource.category.data());
                }

                ImGui::PushID(resource.key.data());

                const bool is_selected = selected_sprite_key == resource.key;
                if (ImGui::Selectable(resource.key.data(), is_selected)) {
                    selected_sprite_key = resource.key;
                    changed = true;
                }

                if (is_selected) {
                    ImGui::SetItemDefaultFocus();
                }

                ImGui::PopID();
            }

            ImGui::EndCombo();
        }

        ImGui::PopID();

        if (const Sprites* selected_resource = find_sprite_resource(selected_sprite_key)) {
            imgui::property_table::text("Sprite Key", selected_resource->key.data());
            imgui::property_table::text_wrapped("Sprite Path", selected_resource->path.data());
        }
        else {
            imgui::property_table::text("Sprite Key", selected_sprite_key.empty() ? "None" : selected_sprite_key.c_str());
            imgui::property_table::text_wrapped("Sprite Path", "Sprite key is not defined in sprite resources");
        }

        return changed;
    }

}
