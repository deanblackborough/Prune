#pragma once

#include <cstdint>
#include <string>

namespace prune {

    using GameObjectId = std::uint32_t;
    constexpr GameObjectId kInvalidGameObjectId = 0;

    enum class GameObjectKind {
        Generic = 0,
        Player,
        Block
    };

    enum class RenderType {
        Rectangle = 0,
        Sprite
    };

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

    struct Size {
        int width = 16;
        int height = 16;
    };

    struct CollisionSettings {
        bool solid = false;
    };

    struct RectangleVisual {
        float color[3] = { 0.3f, 0.8f, 0.5f };
    };

    struct SpriteVisual {
        std::string texture_path;
    };

    struct RenderData {
        RenderType type = RenderType::Rectangle;
        bool visible = true;

        RectangleVisual rectangle{};
        SpriteVisual sprite{};
    };

    struct GameObject {
        GameObjectId id = kInvalidGameObjectId;
        std::string name = "Object";

        GameObjectKind kind = GameObjectKind::Generic;

        Transform transform{};
        Velocity velocity{};
        Size size{};

        CollisionSettings collision{};
        RenderData render{};

        bool active = true;

        [[nodiscard]] RectF bounds() const noexcept;
    };

}
