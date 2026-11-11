#include "prune/scene/authored_store.hpp"

#include "prune/core/overloaded.hpp"

namespace prune {

  void AuthoredStore::capture(const GameObjectManager& live) {
    m_objects = live;
    m_objects.remove_runtime_objects();
  }

  void AuthoredStore::apply(const EditorCommand& command,
                            bool use_after_state) {
    const auto restore = [this](const GameObject& object) {
      restore_object_in_id_order(m_objects, object);
    };

    std::visit(overloaded{
                   [&](const ObjectCreate& payload) {
                     if (use_after_state) {
                       restore(payload.object);
                     } else {
                       m_objects.remove_object(payload.object.identity.id);
                     }
                   },
                   [&](const ObjectDelete& payload) {
                     if (use_after_state) {
                       m_objects.remove_object(payload.object.identity.id);
                     } else {
                       restore(payload.object);
                     }
                   },
                   [&](const ObjectsDelete& payload) {
                     if (use_after_state) {
                       for (const GameObject& object : payload.objects) {
                         m_objects.remove_object(object.identity.id);
                       }
                     } else {
                       for (const GameObject& object : payload.objects) {
                         restore(object);
                       }
                     }
                   },
                   [&](const ObjectsChange& payload) {
                     for (const GameObject& object :
                          use_after_state ? payload.after : payload.before) {
                       restore(object);
                     }
                   },
                   [](const ViewportChange&) {},
                   [](const SceneSettingsChange&) {},
                   [&](const ObjectChange& payload) {
                     restore(use_after_state ? payload.after : payload.before);
                   },
               },
               command.payload);
  }

} // namespace prune
