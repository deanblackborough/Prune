#pragma once

#include "prune/editor/editor_command.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune {

  // The authored baseline: a snapshot of persistent, editor-authored scene
  // data, captured from the live world and kept in sync with it as editor
  // commands are applied, undone, and redone. Never holds runtime-only
  // objects. See REFACTORING.md Phase A (A3) / DECISIONS.md for the model
  // this implements.
  class AuthoredStore {
  public:
    // Replaces the authored snapshot with a copy of `live`, discarding any
    // runtime-only objects.
    void capture(const GameObjectManager& live);

    // Applies (or, if `use_after_state` is false, un-applies) `command`'s
    // object-shaped payload to the authored store. Viewport and scene-
    // settings changes are not object data and are ignored here.
    void apply(const EditorCommand& command, bool use_after_state);

    [[nodiscard]] const GameObjectManager& objects() const noexcept {
      return m_objects;
    }

  private:
    GameObjectManager m_objects;
  };

} // namespace prune
