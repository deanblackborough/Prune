#include "prune/tooling/imgui/property_table.hpp"

namespace prune::tooling::imgui::property_table
{
    constexpr ImGuiTableFlags k_table_flags =
        ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_BordersInnerV;

    void begin_row(const char* label)
    {
        ImGui::TableNextRow();

        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);

        ImGui::TableSetColumnIndex(1);
        ImGui::SetNextItemWidth(-FLT_MIN);
    }

    bool begin(const char* id) {       
        if (!ImGui::BeginTable(id, 2, k_table_flags))
        {;
            return false;
        }

        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.5f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.5f);

        return true;
    }

    void end()
    {
        ImGui::EndTable();
    }

    void label(const char* text)
    {
        begin_row(text);
    }

    bool combo(
        const char* label,
        const char* id,
        int& current_item,
        const char* const items[],
        int items_count
    )
    {
        ImGui::TableNextRow();

        // Label column
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);

        // Value column
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(id);
        const bool changed = ImGui::Combo("##value", &current_item, items, items_count);
        ImGui::PopID();

        return changed;
    }

	bool text(const char* label, const char* value)
	{
		begin_row(label);
		ImGui::TextUnformatted(value);
		return true;
	}

    bool text_wrapped(const char* label, const char* value)
    {
        begin_row(label);
        ImGui::TextWrapped(value);
        return true;
	}

	bool input_text(const char* label, const char* id, char* buffer, size_t buffer_size)
	{
		begin_row(label);
		return ImGui::InputText(id, buffer, buffer_size);
	}

	bool slider_float(const char* label, const char* id, float& value, float min, float max, const char* format)
    {
        begin_row(label);
        return ImGui::SliderFloat(id, &value, min, max, format);
    }

    bool slider_int(const char* label, const char* id, int& value, int min, int max)
    {
        begin_row(label);
        return ImGui::SliderInt(id, &value, min, max);
    }

    bool drag_float(
        const char* label,
        const char* id,
        float& value,
        float speed,
        float min,
        float max,
        const char* format
    )
    {
        ImGui::TableNextRow();

        // Label column
        ImGui::TableSetColumnIndex(0);
        ImGui::TextUnformatted(label);

        // Value column
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(id);

        const bool changed = ImGui::DragFloat(
            "##value",
            &value,
            speed,
            min,
            max,
            format
        );

        ImGui::PopID();

        return changed;
    }

    bool checkbox(const char* label, const char* id, bool& value)
    {
        begin_row(label);
        return ImGui::Checkbox(id, &value);
    }

    bool checkbox_readonly(const char* label, const char* id, bool& value)
    {
        begin_row(label);
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        const bool result = ImGui::Checkbox(id, &value);
        ImGui::PopItemFlag();
        return result;
	}

    bool color3(const char* label, const char* id, float color[3])
    {
        begin_row(label);
        return ImGui::ColorEdit3(id, color);
    }

    bool button(const char* label, const char* text, const ButtonStyle* style)
    {
        begin_row(label);

        int color_count = 0;

        if (style) {
            if (style->normal.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_Button, style->normal.value());
                ++color_count;
            }
            if (style->hovered.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style->hovered.value());
                ++color_count;
            }
            if (style->active.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, style->active.value());
                ++color_count;
            }
        }

        const bool result = ImGui::Button(text);

        if (color_count > 0) {
            ImGui::PopStyleColor(color_count);
        }

        return result;
    }

    bool text_wrapped_raw(const char* text)
    {
        ImGui::TextWrapped(text);
        return true;
	}

    bool button_raw(const char* text, const ButtonStyle* style)
    {
        int color_count = 0;

        if (style) {
            if (style->normal.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_Button, style->normal.value());
                ++color_count;
            }
            if (style->hovered.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style->hovered.value());
                ++color_count;
            }
            if (style->active.has_value()) {
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, style->active.value());
                ++color_count;
            }
        }

        const bool result = ImGui::Button(text);

        if (color_count > 0) {
            ImGui::PopStyleColor(color_count);
        }

        return result;
    }
}