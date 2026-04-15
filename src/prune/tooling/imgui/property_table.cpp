#include <imgui.h>

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
        {
            return false;
        }

        ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthStretch, 0.4f);
        ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch, 0.6f);

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

    bool checkbox(const char* label, const char* id, bool& value)
    {
        begin_row(label);
        return ImGui::Checkbox(id, &value);
    }

    bool color3(const char* label, const char* id, float color[3])
    {
        begin_row(label);
        return ImGui::ColorEdit3(id, color);
    }
}