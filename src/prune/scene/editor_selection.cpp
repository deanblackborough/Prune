#include <algorithm>

#include "prune/scene/editor_selection.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune {

  namespace {
    [[nodiscard]] bool
    can_select_automatically(const GameObject& object) noexcept {
      return object.identity.type != GameObjectType::Runtime &&
             object.editor.selectable;
    }
  } // namespace

  void EditorSelection::select(const GameObjectManager& objects,
                               GameObjectId id) noexcept {
    m_selected_ids.clear();

    if (id == k_invalid_game_object_id) {
      return;
    }

    if (objects.get_by_id(id) != nullptr) {
      m_selected_ids.push_back(id);
    }
  }

  void EditorSelection::select_many(const GameObjectManager& objects,
                                    std::span<const GameObjectId> ids) {
    m_selected_ids.clear();

    for (const GameObjectId id : ids) {
      if (id == k_invalid_game_object_id || objects.get_by_id(id) == nullptr ||
          is_selected(id)) {
        continue;
      }

      m_selected_ids.push_back(id);
    }
  }

  void EditorSelection::toggle_selected(const GameObjectManager& objects,
                                        GameObjectId id) noexcept {
    if (id == k_invalid_game_object_id || objects.get_by_id(id) == nullptr) {
      return;
    }

    const std::size_t index = find_selected_index(id);
    if (index < m_selected_ids.size()) {
      m_selected_ids.erase(m_selected_ids.begin() +
                           static_cast<std::ptrdiff_t>(index));
      return;
    }

    m_selected_ids.push_back(id);
  }

  void EditorSelection::clear_selection() noexcept { m_selected_ids.clear(); }

  void EditorSelection::set_selected_id(const GameObjectManager& objects,
                                        GameObjectId id) noexcept {
    select(objects, id);
  }

  GameObjectId EditorSelection::selected_id() const noexcept {
    if (m_selected_ids.empty()) {
      return k_invalid_game_object_id;
    }

    return m_selected_ids.back();
  }

  bool EditorSelection::is_selected(GameObjectId id) const noexcept {
    return find_selected_index(id) < m_selected_ids.size();
  }

  std::size_t EditorSelection::selected_count() const noexcept {
    return m_selected_ids.size();
  }

  std::span<const GameObjectId> EditorSelection::selected_ids() const noexcept {
    return m_selected_ids;
  }

  GameObject*
  EditorSelection::selected_object(GameObjectManager& objects) const noexcept {
    return objects.get_by_id(selected_id());
  }

  const GameObject* EditorSelection::selected_object(
      const GameObjectManager& objects) const noexcept {
    return objects.get_by_id(selected_id());
  }

  void EditorSelection::object_removed(const GameObjectManager& objects,
                                       GameObjectId id,
                                       std::size_t erased_index) noexcept {
    const bool was_active_selection = selected_id() == id;
    remove_from_selection(id);

    if (!was_active_selection || !m_selected_ids.empty()) {
      return;
    }

    const std::vector<GameObject>& all = objects.objects();

    const auto next =
        std::find_if(all.begin() + static_cast<std::ptrdiff_t>(
                                       std::min(erased_index, all.size())),
                     all.end(), can_select_automatically);

    if (next != all.end()) {
      m_selected_ids.push_back(next->identity.id);
      return;
    }

    const auto previous =
        std::find_if(all.rbegin(), all.rend(), can_select_automatically);

    if (previous != all.rend()) {
      m_selected_ids.push_back(previous->identity.id);
    }
  }

  void EditorSelection::sanitize(const GameObjectManager& objects) noexcept {
    m_selected_ids.erase(
        std::remove_if(m_selected_ids.begin(), m_selected_ids.end(),
                       [&objects](GameObjectId id) {
                         return objects.get_by_id(id) == nullptr;
                       }),
        m_selected_ids.end());
  }

  std::size_t
  EditorSelection::find_selected_index(GameObjectId id) const noexcept {
    for (std::size_t index = 0; index < m_selected_ids.size(); ++index) {
      if (m_selected_ids[index] == id) {
        return index;
      }
    }

    return m_selected_ids.size();
  }

  void EditorSelection::remove_from_selection(GameObjectId id) noexcept {
    m_selected_ids.erase(
        std::remove(m_selected_ids.begin(), m_selected_ids.end(), id),
        m_selected_ids.end());
  }

} // namespace prune
