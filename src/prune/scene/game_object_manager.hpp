#pragma once

#include "prune/scene/game_object.hpp"

#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace prune {

    class GameObjectManager {
    public:
        void clear() noexcept;

        GameObjectId create_object(const GameObject& object);
        bool add_loaded_object(const GameObject& object);
        bool remove_object(GameObjectId id) noexcept;
        std::size_t remove_inactive_runtime_objects(std::string_view behaviour);

        [[nodiscard]] std::size_t count() const noexcept;
        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] GameObject* get_by_id(GameObjectId id) noexcept;
        [[nodiscard]] const GameObject* get_by_id(GameObjectId id) const noexcept;

        [[nodiscard]] GameObject* selected_object() noexcept;
        [[nodiscard]] const GameObject* selected_object() const noexcept;

        void select(GameObjectId id) noexcept;
        void select_many(std::span<const GameObjectId> ids);
        void toggle_selected(GameObjectId id) noexcept;
        void clear_selection() noexcept;
        void set_selected_id(GameObjectId id) noexcept;
        [[nodiscard]] GameObjectId selected_id() const noexcept;
        [[nodiscard]] bool is_selected(GameObjectId id) const noexcept;
        [[nodiscard]] std::size_t selected_count() const noexcept;
        [[nodiscard]] std::span<const GameObjectId> selected_ids() const noexcept;

        void set_next_id(GameObjectId next_id) noexcept;
        [[nodiscard]] GameObjectId next_id() const noexcept;

        [[nodiscard]] std::vector<GameObject>& objects() noexcept;
        [[nodiscard]] const std::vector<GameObject>& objects() const noexcept;

        [[nodiscard]] std::string make_unique_name(std::string desired, GameObjectId ignore_id) const;

    private:
        [[nodiscard]] std::size_t find_index_by_id(GameObjectId id) const noexcept;
        [[nodiscard]] std::size_t find_selected_index(GameObjectId id) const noexcept;
        void remove_from_selection(GameObjectId id) noexcept;
        void sanitize_selection() noexcept;

        std::vector<GameObject> m_objects;
        std::vector<GameObjectId> m_selected_ids;
        GameObjectId m_next_id = 1;
    };
}
