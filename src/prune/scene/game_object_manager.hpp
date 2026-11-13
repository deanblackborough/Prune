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
    std::size_t remove_runtime_objects();
    std::size_t remove_inactive_runtime_objects(std::string_view behaviour);

    [[nodiscard]] std::size_t count() const noexcept;
    [[nodiscard]] bool empty() const noexcept;

    [[nodiscard]] GameObject* get_by_id(GameObjectId id) noexcept;
    [[nodiscard]] const GameObject* get_by_id(GameObjectId id) const noexcept;
    [[nodiscard]] std::size_t index_of(GameObjectId id) const noexcept;

    void set_next_id(GameObjectId next_id) noexcept;
    [[nodiscard]] GameObjectId next_id() const noexcept;

    [[nodiscard]] std::vector<GameObject>& objects() noexcept;
    [[nodiscard]] const std::vector<GameObject>& objects() const noexcept;

    [[nodiscard]] std::string make_unique_name(std::string desired,
                                               GameObjectId ignore_id) const;

  private:
    std::vector<GameObject> m_objects;
    GameObjectId m_next_id = 1;
  };

  // Inserts `object` into `objects`, replacing any existing object with the
  // same id, or adding it while keeping `objects` ordered by id. Used to
  // restore a snapshot captured before a create/delete/change, so replaying
  // commands leaves objects in a stable, deterministic order.
  void restore_object_in_id_order(GameObjectManager& objects,
                                  const GameObject& object);

  // Same as calling restore_object_in_id_order for each of `restored`, but
  // sorts `objects` at most once instead of once per restored object.
  void restore_objects_in_id_order(GameObjectManager& objects,
                                   std::span<const GameObject> restored);

} // namespace prune
