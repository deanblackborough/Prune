#pragma once

#include <string>

namespace prune {

    struct Transform {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct RectF {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
    };

    struct RectangleVisual {
        int width = 50;
        int height = 50;
        float color[3] = {0.3f, 0.8f, 0.5f};
    };

    struct GameObject {
        std::string name = "Object";
        Transform transform{};
        RectangleVisual rectangle{};

        [[nodiscard]] RectF bounds() const noexcept;
        void clamp_to_area(int area_width, int area_height) noexcept;
    };

} // namespace prune