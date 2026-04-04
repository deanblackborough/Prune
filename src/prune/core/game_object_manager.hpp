#pragma once

#include "game_object.hpp"

#include <cstddef>
#include <vector>

namespace prune {

    class GameObjectManager {
    public:
        void clear() noexcept;

        std::size_t create_object(const GameObject& object);

        [[nodiscard]] std::size_t count() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] GameObject* get(std::size_t index) noexcept;
        [[nodiscard]] const GameObject* get(std::size_t index) const noexcept;

        [[nodiscard]] GameObject* selected_object() noexcept;
        [[nodiscard]] const GameObject* selected_object() const noexcept;

        void select(std::size_t index) noexcept;
        [[nodiscard]] std::size_t selected_index() const noexcept;

        [[nodiscard]] std::vector<GameObject>& objects() noexcept;
        [[nodiscard]] const std::vector<GameObject>& objects() const noexcept;

    private:
        std::vector<GameObject> m_objects;
        std::size_t m_selected_index = 0;
    };

} // namespace prune