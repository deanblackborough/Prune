#include "prune/scene/authored_store.hpp"

namespace prune {

  namespace {
    struct AuthoredStoreOps {
      GameObjectManager& objects;

      void restore_one(const GameObject& object) {
        restore_object_in_id_order(objects, object);
      }

      void remove_one(GameObjectId id) { objects.remove_object(id); }

      void restore_many(std::span<const GameObject> batch) {
        restore_objects_in_id_order(objects, batch);
      }

      void after_objects_removed() {}
    };
  } // namespace

  void AuthoredStore::capture(const GameObjectManager& live) {
    m_objects = live;
    m_objects.remove_runtime_objects();
  }

  void AuthoredStore::apply(const EditorCommand& command,
                            bool use_after_state) {
    AuthoredStoreOps ops{m_objects};
    apply_object_payload(command, use_after_state, ops);
  }

} // namespace prune
