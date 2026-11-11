#include <algorithm>

#include "prune/scene/game_object_manager.hpp"

namespace prune {

  void GameObjectManager::clear() noexcept {
    m_objects.clear();
    m_next_id = 1;
  }

  GameObjectId GameObjectManager::create_object(const GameObject& object) {
    GameObject copy = object;
    copy.identity.id = m_next_id++;

    m_objects.push_back(copy);
    return copy.identity.id;
  }

  std::size_t GameObjectManager::remove_runtime_objects() {
    const std::size_t before = m_objects.size();

    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(),
                                   [](const GameObject& object) {
                                     return object.identity.type ==
                                            GameObjectType::Runtime;
                                   }),
                    m_objects.end());

    return before - m_objects.size();
  }

  std::size_t GameObjectManager::remove_inactive_runtime_objects(
      std::string_view behaviour) {
    const std::size_t before = m_objects.size();

    m_objects.erase(std::remove_if(m_objects.begin(), m_objects.end(),
                                   [behaviour](const GameObject& object) {
                                     return object.identity.type ==
                                                GameObjectType::Runtime &&
                                            !object.runtime.persistent &&
                                            object.runtime.behaviour ==
                                                behaviour &&
                                            !object.lifecycle.active;
                                   }),
                    m_objects.end());

    return before - m_objects.size();
  }

  bool GameObjectManager::add_loaded_object(const GameObject& object) {
    if (object.identity.id == k_invalid_game_object_id) {
      return false;
    }

    if (get_by_id(object.identity.id) != nullptr) {
      return false;
    }

    m_objects.push_back(object);
    m_next_id = std::max(m_next_id, object.identity.id + 1);
    return true;
  }

  void GameObjectManager::set_next_id(GameObjectId next_id) noexcept {
    m_next_id = std::max<GameObjectId>(1, next_id);
  }

  GameObjectId GameObjectManager::next_id() const noexcept { return m_next_id; }

  bool GameObjectManager::remove_object(GameObjectId id) noexcept {
    const std::size_t index = index_of(id);
    if (index >= m_objects.size()) {
      return false;
    }

    m_objects.erase(m_objects.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
  }

  std::size_t GameObjectManager::count() const noexcept {
    return m_objects.size();
  }

  bool GameObjectManager::empty() const noexcept { return m_objects.empty(); }

  GameObject* GameObjectManager::get_by_id(GameObjectId id) noexcept {
    const std::size_t index = index_of(id);
    if (index >= m_objects.size()) {
      return nullptr;
    }

    return &m_objects[index];
  }

  const GameObject*
  GameObjectManager::get_by_id(GameObjectId id) const noexcept {
    const std::size_t index = index_of(id);
    if (index >= m_objects.size()) {
      return nullptr;
    }

    return &m_objects[index];
  }

  std::size_t GameObjectManager::index_of(GameObjectId id) const noexcept {
    if (id == k_invalid_game_object_id) {
      return m_objects.size();
    }

    for (std::size_t index = 0; index < m_objects.size(); ++index) {
      if (m_objects[index].identity.id == id) {
        return index;
      }
    }

    return m_objects.size();
  }

  std::vector<GameObject>& GameObjectManager::objects() noexcept {
    return m_objects;
  }

  const std::vector<GameObject>& GameObjectManager::objects() const noexcept {
    return m_objects;
  }

  void restore_object_in_id_order(GameObjectManager& objects,
                                  const GameObject& object) {
    if (GameObject* existing = objects.get_by_id(object.identity.id)) {
      *existing = object;
      return;
    }

    objects.add_loaded_object(object);
    std::ranges::stable_sort(
        objects.objects(), {},
        [](const GameObject& candidate) { return candidate.identity.id; });
  }

  std::string
  GameObjectManager::make_unique_name(std::string desired,
                                      GameObjectId ignore_id) const {
    if (desired.empty()) {
      desired = "Object";
    }

    auto is_taken = [&](const std::string& name) {
      for (const auto& obj : objects()) {
        if (obj.identity.id != ignore_id && obj.identity.name == name) {
          return true;
        }
      }
      return false;
    };

    if (!is_taken(desired)) {
      return desired;
    }

    int suffix = 1;
    std::string candidate;

    do {
      candidate = desired + " " + std::to_string(suffix++);
    } while (is_taken(candidate));

    return candidate;
  }

} // namespace prune
