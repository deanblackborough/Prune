#include "game_object_manager.hpp"

namespace prune {

    void GameObjectManager::clear() noexcept
    {
        m_objects.clear();
        m_selected_index = 0;
    }

    std::size_t GameObjectManager::create_object(const GameObject& object)
    {
        m_objects.push_back(object);

        if (m_objects.size() == 1) {
            m_selected_index = 0;
        }

        return m_objects.size() - 1;
    }

    std::size_t GameObjectManager::count() const noexcept
    {
        return m_objects.size();
    }

    bool GameObjectManager::empty() const noexcept
    {
        return m_objects.empty();
    }

    GameObject* GameObjectManager::get(std::size_t index) noexcept
    {
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    const GameObject* GameObjectManager::get(std::size_t index) const noexcept
    {
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    GameObject* GameObjectManager::selected_object() noexcept
    {
        return get(m_selected_index);
    }

    const GameObject* GameObjectManager::selected_object() const noexcept
    {
        return get(m_selected_index);
    }

    void GameObjectManager::select(std::size_t index) noexcept
    {
        if (index < m_objects.size()) {
            m_selected_index = index;
        }
    }

    std::size_t GameObjectManager::selected_index() const noexcept
    {
        return m_selected_index;
    }

    std::vector<GameObject>& GameObjectManager::objects() noexcept
    {
        return m_objects;
    }

    const std::vector<GameObject>& GameObjectManager::objects() const noexcept
    {
        return m_objects;
    }

} // namespace prune