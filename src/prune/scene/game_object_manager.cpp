#include "prune/scene/game_object_manager.hpp"

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

    bool GameObjectManager::remove_object(GameObjectId id) noexcept
    {
        const std::size_t index = find_index_by_id(id);
        if (index >= m_objects.size()) {
            return false;
        }

        m_objects.erase(m_objects.begin() + static_cast<std::ptrdiff_t>(index));

        if (m_selected_id == id) {
            if (m_objects.empty()) {
                m_selected_id = kInvalidGameObjectId;
            } else if (index < m_objects.size()) {
                m_selected_id = m_objects[index].id;
            } else {
                m_selected_id = m_objects.back().id;
            }
        }

        return true;
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
        if (id == kInvalidGameObjectId) {
            m_selected_id = kInvalidGameObjectId;
            return;
        }

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

    std::string GameObjectManager::make_unique_name(std::string desired, GameObjectId ignore_id) const {
        if (desired.empty()) {
            desired = "Object";
        }

        auto is_taken = [&](const std::string& name) {
            for (const auto& obj : objects()) {
                if (obj.id != ignore_id && obj.name == name) {
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