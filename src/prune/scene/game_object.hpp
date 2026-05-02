#pragma once

#include <cstdint>
#include <string>

#include "prune/core/defaults.hpp"

namespace prune {

    using GameObjectId = std::uint32_t;
    constexpr GameObjectId k_invalid_game_object_id = 0;

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
        int width = k_default_object_size;
        int height = k_default_object_size;
    };

    struct CollisionSettings {
        bool solid = false;
    };

    struct RectangleVisual {
        float color[3] = { 0.3f, 0.8f, 0.5f };
    };

    struct SpriteVisual {
        std::string sprite_key = std::string(k_default_sprite_key);
    };

    struct RenderData {
        RenderType type = RenderType::Rectangle;
        bool visible = true;

        RectangleVisual rectangle{};
        SpriteVisual sprite{};
    };

    struct GameObject {
        GameObjectId id = k_invalid_game_object_id;
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
