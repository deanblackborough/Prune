#pragma once

namespace prune::tooling::imgui::property_table {

    void begin_row(const char* label);

    bool begin(const char* id);
    void end();

    void label(const char* text);

    bool slider_float(const char* label, const char* id, float& value, float min, float max, const char* format = "%.2f");
    bool slider_int(const char* label, const char* id, int& value, int min, int max);
    bool checkbox(const char* label, const char* id, bool& value);
    bool color3(const char* label, const char* id, float color[3]);

}
