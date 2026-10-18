#include "imgui.h"

#include "prune/scene/scene.hpp"
#include "prune/tooling/editor/tracked_property_table.hpp"
#include "prune/tooling/imgui/property_table.hpp"
#include "prune/tooling/sprite_picker.hpp"

namespace prune::tooling::editor::tracked_property_table {

  namespace {

    [[nodiscard]] bool command_object_changed(TrackedField field,
                                              const GameObject& before,
                                              const GameObject& after) noexcept {
      switch (field) {
      case TrackedField::Position:
        return before.transform.x != after.transform.x ||
               before.transform.y != after.transform.y;

      case TrackedField::Size:
        return before.size.width != after.size.width ||
               before.size.height != after.size.height;

      case TrackedField::RenderType:
        return before.render.type != after.render.type;

      case TrackedField::Colour:
        return before.render.rectangle.color[0] !=
                   after.render.rectangle.color[0] ||
               before.render.rectangle.color[1] !=
                   after.render.rectangle.color[1] ||
               before.render.rectangle.color[2] !=
                   after.render.rectangle.color[2];

      case TrackedField::ZIndex:
        return before.render.z_index != after.render.z_index;

      case TrackedField::Flag:
        return before.lifecycle.active != after.lifecycle.active ||
               before.render.visible != after.render.visible ||
               before.collision.solid != after.collision.solid;

      case TrackedField::Sprite:
        return before.render.sprite.sprite_key !=
                   after.render.sprite.sprite_key ||
               before.render.sprite.flip_x != after.render.sprite.flip_x;

      case TrackedField::Rename:
        return before.identity.name != after.identity.name;
      }

      return true;
    }

    [[nodiscard]] const char* tracked_field_label(TrackedField field) noexcept {
      switch (field) {
      case TrackedField::Position:
        return "Change object position";
      case TrackedField::Size:
        return "Change object size";
      case TrackedField::RenderType:
        return "Change object rendering type";
      case TrackedField::Colour:
        return "Change object colour";
      case TrackedField::ZIndex:
        return "Change render order";
      case TrackedField::Flag:
        return "Change object flag";
      case TrackedField::Sprite:
        return "Change sprite";
      case TrackedField::Rename:
        return "Rename object";
      }

      return "Change object";
    }

  } // namespace

  void ObjectEditTracker::capture_if_activated(const GameObject& before) {
    if (ImGui::IsItemActivated()) {
      m_before = before;
    }
  }

  void ObjectEditTracker::commit_if_deactivated_after_edit(
      Scene& scene, TrackedField field, const GameObject& after,
      std::string_view detail) {
    if (!ImGui::IsItemDeactivatedAfterEdit()) {
      return;
    }

    if (!m_before.has_value()) {
      return;
    }

    const GameObject before = m_before.value();
    m_before.reset();

    commit_if_changed(scene, field, before, after, detail);
  }

  void ObjectEditTracker::reset() noexcept { m_before.reset(); }

  bool drag_float(ObjectEditTracker& tracker, Scene& scene, TrackedField field,
                  GameObject& object, const char* label, const char* id,
                  float& value, float speed, float min, float max,
                  const char* format, std::string_view detail) {
    const GameObject before = object;
    const bool changed = ::prune::tooling::imgui::property_table::drag_float(
        label, id, value, speed, min, max, format);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, field, object, detail);

    return changed;
  }

  bool slider_int(ObjectEditTracker& tracker, Scene& scene, TrackedField field,
                  GameObject& object, const char* label, const char* id,
                  int& value, int min, int max, std::string_view detail) {
    const GameObject before = object;
    const bool changed = ::prune::tooling::imgui::property_table::slider_int(
        label, id, value, min, max);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, field, object, detail);

    return changed;
  }

  bool drag_int(ObjectEditTracker& tracker, Scene& scene, TrackedField field,
                GameObject& object, const char* label, const char* id,
                int& value, float speed, int min, int max,
                std::string_view detail) {
    const GameObject before = object;
    const bool changed = ::prune::tooling::imgui::property_table::drag_int(
        label, id, value, speed, min, max);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, field, object, detail);

    return changed;
  }

  bool color3(ObjectEditTracker& tracker, Scene& scene, TrackedField field,
              GameObject& object, const char* label, const char* id,
              float color[3], std::string_view detail) {
    const GameObject before = object;
    const bool changed =
        ::prune::tooling::imgui::property_table::color3(label, id, color);

    tracker.capture_if_activated(before);
    tracker.commit_if_deactivated_after_edit(scene, field, object, detail);

    return changed;
  }

  bool checkbox(Scene& scene, TrackedField field, GameObject& object,
                const char* label, const char* id, bool& value,
                std::string_view detail) {
    const GameObject before = object;

    if (!::prune::tooling::imgui::property_table::checkbox(label, id, value)) {
      return false;
    }

    commit_if_changed(scene, field, before, object, detail);
    return true;
  }

  bool sprite_picker(Scene& scene, TrackedField field, GameObject& object,
                     const char* label, const char* id, std::string& sprite_key,
                     std::string_view detail) {
    const GameObject before = object;

    if (!::prune::tooling::draw_sprite_picker(label, id, sprite_key)) {
      return false;
    }

    commit_if_changed(scene, field, before, object, detail);
    return true;
  }

  bool commit_if_changed(Scene& scene, TrackedField field,
                         const GameObject& before, const GameObject& after,
                         std::string_view detail) {
    if (before.identity.id != after.identity.id ||
        !command_object_changed(field, before, after)) {
      return false;
    }

    scene.record_editor_command(
        make_object_change(tracked_field_label(field), before, after, detail));

    return true;
  }
} // namespace prune::tooling::editor::tracked_property_table
