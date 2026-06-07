#include <algorithm>

#include "prune/scene/game_object_manager.hpp"

namespace prune {

    void GameObjectManager::clear() noexcept
    {
        m_objects.clear();
        m_selected_ids.clear();
        m_next_id = 1;
    }

    GameObjectId GameObjectManager::create_object(const GameObject& object)
    {
        GameObject copy = object;
        copy.identity.id = m_next_id++;

        m_objects.push_back(copy);

        if (m_selected_ids.empty()) {
            m_selected_ids.push_back(copy.identity.id);
        }

        return copy.identity.id;
    }

    std::size_t GameObjectManager::remove_inactive_runtime_objects(std::string_view behaviour)
    {
        const std::size_t before = m_objects.size();

        m_objects.erase(
            std::remove_if(
                m_objects.begin(),
                m_objects.end(),
                [behaviour](const GameObject& object) {
                    return object.identity.type == GameObjectType::Runtime &&
                        !object.runtime.persistent &&
                        object.runtime.behaviour == behaviour &&
                        !object.lifecycle.active;
                }
            ),
            m_objects.end()
        );

        sanitize_selection();

        return before - m_objects.size();
    }

    bool GameObjectManager::add_loaded_object(const GameObject& object)
    {
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

    void GameObjectManager::set_selected_id(GameObjectId id) noexcept
    {
        select(id);
    }

    void GameObjectManager::set_next_id(GameObjectId next_id) noexcept
    {
        m_next_id = std::max<GameObjectId>(1, next_id);
    }

    GameObjectId GameObjectManager::next_id() const noexcept
    {
        return m_next_id;
    }

    bool GameObjectManager::remove_object(GameObjectId id) noexcept
    {
        const std::size_t index = find_index_by_id(id);
        if (index >= m_objects.size()) {
            return false;
        }

        const bool was_active_selection = selected_id() == id;

        m_objects.erase(m_objects.begin() + static_cast<std::ptrdiff_t>(index));
        remove_from_selection(id);

        if (was_active_selection && m_selected_ids.empty()) {
            if (m_objects.empty()) {
                return true;
            }

            if (index < m_objects.size()) {
                m_selected_ids.push_back(m_objects[index].identity.id);
            }
            else {
                m_selected_ids.push_back(m_objects.back().identity.id);
            }
        }

        sanitize_selection();
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
        return get_by_id(selected_id());
    }

    const GameObject* GameObjectManager::selected_object() const noexcept
    {
        return get_by_id(selected_id());
    }

    void GameObjectManager::select(GameObjectId id) noexcept
    {
        m_selected_ids.clear();

        if (id == k_invalid_game_object_id) {
            return;
        }

        if (get_by_id(id) != nullptr) {
            m_selected_ids.push_back(id);
        }
    }

    void GameObjectManager::toggle_selected(GameObjectId id) noexcept
    {
        if (id == k_invalid_game_object_id || get_by_id(id) == nullptr) {
            return;
        }

        const std::size_t index = find_selected_index(id);
        if (index < m_selected_ids.size()) {
            m_selected_ids.erase(m_selected_ids.begin() + static_cast<std::ptrdiff_t>(index));
            return;
        }

        m_selected_ids.push_back(id);
    }

    void GameObjectManager::clear_selection() noexcept
    {
        m_selected_ids.clear();
    }

    GameObjectId GameObjectManager::selected_id() const noexcept
    {
        if (m_selected_ids.empty()) {
            return k_invalid_game_object_id;
        }

        return m_selected_ids.back();
    }

    bool GameObjectManager::is_selected(GameObjectId id) const noexcept
    {
        return find_selected_index(id) < m_selected_ids.size();
    }

    std::size_t GameObjectManager::selected_count() const noexcept
    {
        return m_selected_ids.size();
    }

    std::span<const GameObjectId> GameObjectManager::selected_ids() const noexcept
    {
        return m_selected_ids;
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

    std::size_t GameObjectManager::find_index_by_id(GameObjectId id) const noexcept
    {
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

    std::size_t GameObjectManager::find_selected_index(GameObjectId id) const noexcept
    {
        for (std::size_t index = 0; index < m_selected_ids.size(); ++index) {
            if (m_selected_ids[index] == id) {
                return index;
            }
        }

        return m_selected_ids.size();
    }

    void GameObjectManager::remove_from_selection(GameObjectId id) noexcept
    {
        m_selected_ids.erase(
            std::remove(m_selected_ids.begin(), m_selected_ids.end(), id),
            m_selected_ids.end()
        );
    }

    void GameObjectManager::sanitize_selection() noexcept
    {
        m_selected_ids.erase(
            std::remove_if(
                m_selected_ids.begin(),
                m_selected_ids.end(),
                [this](GameObjectId id) {
                    return get_by_id(id) == nullptr;
                }
            ),
            m_selected_ids.end()
        );
    }

}
