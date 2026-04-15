#pragma once

#include <imgui.h>
#include <optional>

namespace prune::tooling::imgui::property_table {

    struct ButtonStyle {
        std::optional<ImVec4> normal;
        std::optional<ImVec4> hovered;
        std::optional<ImVec4> active;
    };

    void begin_row(const char* label);

    bool begin(const char* id);
    void end();

    void label(const char* text);

	bool text(const char* label, const char* value);
	bool input_text(const char* label, const char* id, char* buffer, size_t buffer_size);
	bool slider_float(const char* label, const char* id, float& value, float min, float max, const char* format = "%.2f");
    bool slider_int(const char* label, const char* id, int& value, int min, int max);
    bool checkbox(const char* label, const char* id, bool& value);
    bool color3(const char* label, const char* id, float color[3]);
    bool button(const char* label, const char* text, const ButtonStyle* style = nullptr);
    bool button_raw(const char* text, const ButtonStyle* style = nullptr);

}
