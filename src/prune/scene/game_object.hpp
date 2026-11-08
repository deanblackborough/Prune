#pragma once

#include <cstdint>
#include <string>

#include "prune/core/defaults.hpp"

namespace prune {

  using GameObjectId = std::uint32_t;
  constexpr GameObjectId k_invalid_game_object_id = 0;

  enum class GameObjectType { Authored = 0, Runtime };

  struct EditorFlags {
    bool selectable = true;
    bool renameable = true;
    bool movable = true;
    bool deletable = true;
    bool cloneable = true;
  };

  struct RuntimeData {
    bool persistent = true;
    std::string behaviour;
  };

  enum class Direction { Up = 0, Down, Left, Right };

  enum class RenderType { Rectangle = 0, Sprite };

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
    float color[3] = {0.3f, 0.8f, 0.5f};
  };

  struct SpriteVisual {
    std::string sprite_key = std::string(k_default_sprite_key);
    bool flip_x = false;
  };

  struct RenderData {
    RenderType type = RenderType::Rectangle;
    bool visible = true;
    int z_index = 0;

    RectangleVisual rectangle{};
    SpriteVisual sprite{};
  };

  struct ObjectIdentity {
    GameObjectId id = k_invalid_game_object_id;
    std::string name = "Object";
    GameObjectType type = GameObjectType::Authored;
  };

  // The authored concept this object represents (e.g. "platformer.player"),
  // set once by the owning scene's factory. Distinct from runtime.behaviour:
  // this answers "what is this object", not "what runtime behaviour does it
  // currently run" — the two happen to share values today, but behaviour
  // toggles will need them to vary independently.
  struct ObjectSemantics {
    std::string concept_id;
  };

  struct ObjectMotion {
    Velocity velocity{};
    Direction facing = Direction::Right;
  };

  struct ObjectLifecycle {
    bool active = true;
    float remaining = 0.0f;
  };

  struct GameObject {
    ObjectIdentity identity{};
    ObjectSemantics semantics{};

    Transform transform{};
    ObjectMotion motion{};
    Size size{};

    CollisionSettings collision{};
    RenderData render{};

    EditorFlags editor{};
    RuntimeData runtime{};
    ObjectLifecycle lifecycle{};

    [[nodiscard]] RectF bounds() const noexcept;
  };

} // namespace prune
