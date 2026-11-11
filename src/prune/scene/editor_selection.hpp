#pragma once

#include "prune/scene/game_object.hpp"

#include <cstddef>
#include <span>
#include <vector>

namespace prune {

  class GameObjectManager;

  // Editor-session selection state: which live objects are selected. Never
  // part of authored/serialized data, and never held by GameObjectManager
  // itself (object ownership and storage only).
  class EditorSelection {
  public:
    void select(const GameObjectManager& objects, GameObjectId id) noexcept;
    void select_many(const GameObjectManager& objects,
                     std::span<const GameObjectId> ids);
    void toggle_selected(const GameObjectManager& objects,
                         GameObjectId id) noexcept;
    void clear_selection() noexcept;
    void set_selected_id(const GameObjectManager& objects,
                         GameObjectId id) noexcept;

    [[nodiscard]] GameObjectId selected_id() const noexcept;
    [[nodiscard]] bool is_selected(GameObjectId id) const noexcept;
    [[nodiscard]] std::size_t selected_count() const noexcept;
    [[nodiscard]] std::span<const GameObjectId> selected_ids() const noexcept;

    [[nodiscard]] GameObject*
    selected_object(GameObjectManager& objects) const noexcept;
    [[nodiscard]] const GameObject*
    selected_object(const GameObjectManager& objects) const noexcept;

    // Called right after `id` has been erased from `objects`, with the index
    // it occupied just before removal, to preserve the "select the next, or
    // otherwise previous, selectable object" fallback when the active
    // selection is deleted.
    void object_removed(const GameObjectManager& objects, GameObjectId id,
                        std::size_t erased_index) noexcept;

    // Drops any selected id that no longer exists in `objects`.
    void sanitize(const GameObjectManager& objects) noexcept;

  private:
    [[nodiscard]] std::size_t
    find_selected_index(GameObjectId id) const noexcept;
    void remove_from_selection(GameObjectId id) noexcept;

    std::vector<GameObjectId> m_selected_ids;
  };

} // namespace prune
