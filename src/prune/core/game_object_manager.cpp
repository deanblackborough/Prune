#include "game_object_manager.hpp"

namespace prune {

    /**
     * @brief Remove all managed game objects and reset the selection.
     *
     * Clears the internal object container and sets the selected index to 0.
     */
    void GameObjectManager::clear() noexcept
    {
        m_objects.clear();
        m_selected_index = 0;
    }

    /**
     * @brief Appends a GameObject to the manager and returns its index.
     *
     * Adds a copy of `object` to the internal container. If this is the first
     * object added, the selected index is set to 0.
     *
     * @param object The GameObject to add (copied into the manager).
     * @return std::size_t Index of the newly added object within the container.
     */
    std::size_t GameObjectManager::create_object(const GameObject& object)
    {
        m_objects.push_back(object);

        if (m_objects.size() == 1) {
            m_selected_index = 0;
        }

        return m_objects.size() - 1;
    }

    /**
     * @brief Returns the number of managed game objects.
     *
     * @return std::size_t The current number of objects stored in the manager.
     */
    std::size_t GameObjectManager::count() const noexcept
    {
        return m_objects.size();
    }

    /**
     * @brief Reports whether the manager contains no game objects.
     *
     * @return `true` if there are no objects managed, `false` otherwise.
     */
    bool GameObjectManager::empty() const noexcept
    {
        return m_objects.empty();
    }

    /**
     * @brief Retrieve a mutable pointer to the GameObject at the given index.
     *
     * @param index Zero-based position of the desired object in the internal container.
     * @return GameObject* Pointer to the object at `index`, or `nullptr` if `index` is out of range.
     */
    GameObject* GameObjectManager::get(std::size_t index) noexcept
    {
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    /**
     * Retrieve the object at the specified index.
     *
     * @param index Zero-based index of the object to retrieve.
     * @return `nullptr` if `index` is out of range, otherwise a pointer to the `GameObject` at `index`.
     */
    const GameObject* GameObjectManager::get(std::size_t index) const noexcept
    {
        if (index >= m_objects.size()) {
            return nullptr;
        }

        return &m_objects[index];
    }

    /**
     * @brief Accesses the currently selected game object.
     *
     * @return GameObject* Pointer to the selected GameObject, or `nullptr` if the selected index is out of range or the manager contains no objects.
     */
    GameObject* GameObjectManager::selected_object() noexcept
    {
        return get(m_selected_index);
    }

    /**
     * @brief Retrieve the currently selected object.
     *
     * @return const GameObject* Pointer to the selected GameObject, or `nullptr` if no object is selected or the selected index is out of range.
     */
    const GameObject* GameObjectManager::selected_object() const noexcept
    {
        return get(m_selected_index);
    }

    /**
     * @brief Set the currently selected object index if it is valid.
     *
     * If `index` is less than the number of managed objects, updates the selected
     * index to `index`; otherwise leaves the current selection unchanged.
     *
     * @param index Desired index to select.
     */
    void GameObjectManager::select(std::size_t index) noexcept
    {
        if (index < m_objects.size()) {
            m_selected_index = index;
        }
    }

    /**
     * @brief Retrieves the index of the currently selected GameObject.
     *
     * @return std::size_t Index of the selected GameObject within the manager's container.
     */
    std::size_t GameObjectManager::selected_index() const noexcept
    {
        return m_selected_index;
    }

    /**
     * @brief Access the internal container of managed game objects.
     *
     * Provides a mutable reference to the manager's internal vector so callers can
     * iterate, modify, or reorder the stored GameObject instances.
     *
     * @return std::vector<GameObject>& Reference to the internal container of managed game objects.
     */
    std::vector<GameObject>& GameObjectManager::objects() noexcept
    {
        return m_objects;
    }

    /**
     * @brief Accesses the container of managed GameObject instances.
     *
     * Provides a const reference to the internal vector that stores all managed
     * GameObject objects. The reference remains valid until the GameObjectManager
     * is modified (e.g., objects added/removed) or destroyed.
     *
     * @return const std::vector<GameObject>& Const reference to the internal object list.
     */
    const std::vector<GameObject>& GameObjectManager::objects() const noexcept
    {
        return m_objects;
    }

} // namespace prune