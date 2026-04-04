#include "game_object_manager.hpp"

namespace prune {

    void GameObjectManager::clear() noexcept
    {
        m_objects.clear();
        m_next_id = 1;
        m_selected_id = kInvalidGameObjectId;
    }

    GameObjectId GameObjectManager::create_object(const GameObject& object)
    {
        GameObject copy = object;
        copy.id = m_next_id++;

        m_objects.push_back(copy);

        if (m_selected_id == kInvalidGameObjectId) {
            m_selected_id = copy.id;
        }

        return copy.id;
    }

    std::size_t GameObjectManager::count() const noexcept
    {
        return m_objects.size();
    }

    bool GameObjectManager::empty() const noexcept
    {
        return m_objects.empty();
    }

    GameObject* GameObjectManager::get_by_id(GameObjectId id) noexcept
    {
        const std::size_t index = find_index_by_id(id);
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    const GameObject* GameObjectManager::get_by_id(GameObjectId id) const noexcept
    {
        const std::size_t index = find_index_by_id(id);
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    GameObject* GameObjectManager::selected_object() noexcept
    {
        return get_by_id(m_selected_id);
    }

    const GameObject* GameObjectManager::selected_object() const noexcept
    {
        return get_by_id(m_selected_id);
    }

    void GameObjectManager::select(GameObjectId id) noexcept
    {
        if (get_by_id(id) != nullptr) {
            m_selected_id = id;
        }
    }

    GameObjectId GameObjectManager::selected_id() const noexcept
    {
        return m_selected_id;
    }

    std::vector<GameObject>& GameObjectManager::objects() noexcept
    {
        return m_objects;
    }

    const std::vector<GameObject>& GameObjectManager::objects() const noexcept
    {
        return m_objects;
    }

    std::size_t GameObjectManager::find_index_by_id(GameObjectId id) const noexcept
    {
        if (id == kInvalidGameObjectId) {
            return m_objects.size();
        }

        for (std::size_t index = 0; index < m_objects.size(); ++index) {
            if (m_objects[index].id == id) {
                return index;
            }
        }

        return m_objects.size();
    }

} // namespace prune