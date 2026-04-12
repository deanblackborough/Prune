#pragma once

#include "prune/scene/game_object.hpp"

#include <vector>

namespace prune {

    class GameObjectManager {
    public:
        void clear() noexcept;

        GameObjectId create_object(const GameObject& object);
        bool remove_object(GameObjectId id) noexcept;

        [[nodiscard]] std::size_t count() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] GameObject* get_by_id(GameObjectId id) noexcept;
        [[nodiscard]] const GameObject* get_by_id(GameObjectId id) const noexcept;

        [[nodiscard]] GameObject* selected_object() noexcept;
        [[nodiscard]] const GameObject* selected_object() const noexcept;

        void select(GameObjectId id) noexcept;
        [[nodiscard]] GameObjectId selected_id() const noexcept;

        [[nodiscard]] std::vector<GameObject>& objects() noexcept;
        [[nodiscard]] const std::vector<GameObject>& objects() const noexcept;

        [[nodiscard]] std::string make_unique_name(std::string desired, GameObjectId ignore_id) const;

    private:
        [[nodiscard]] std::size_t find_index_by_id(GameObjectId id) const noexcept;

        std::vector<GameObject> m_objects;
        GameObjectId m_next_id = 1;
        GameObjectId m_selected_id = kInvalidGameObjectId;
    };

} // namespace prune