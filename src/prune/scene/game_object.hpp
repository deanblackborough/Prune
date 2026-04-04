#pragma once

#include <cstdint>
#include <string>

namespace prune {

    using GameObjectId = std::uint32_t;
    constexpr GameObjectId kInvalidGameObjectId = 0;

    struct Transform {
        float x = 0.0f;
        float y = 0.0f;
    };

    struct Velocity {
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
        GameObjectId id = kInvalidGameObjectId;
        std::string name = "Object";

        Transform transform{};
        Velocity velocity{};
        RectangleVisual rectangle{};

        bool active = true;
        bool visible = true;
        bool solid = false;
        bool is_player = false;

        [[nodiscard]] RectF bounds() const noexcept;
        void clamp_to_area(int area_width, int area_height) noexcept;
    };

} // namespace prune