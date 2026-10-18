#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"

namespace prune {

  struct ObjectChange {
    GameObject before;
    GameObject after;
  };

  struct ObjectsChange {
    std::vector<GameObject> before;
    std::vector<GameObject> after;
  };

  struct ObjectCreate {
    GameObject object;
  };

  struct ObjectDelete {
    GameObject object;
  };

  struct ObjectsDelete {
    std::vector<GameObject> objects;
  };

  struct ViewportChange {
    Camera before;
    Camera after;
  };

  using EditorCommandPayload =
      std::variant<ObjectChange, ObjectsChange, ObjectCreate, ObjectDelete,
                   ObjectsDelete, ViewportChange>;

  struct EditorCommand {
    std::string label;
    std::string detail;
    bool makes_dirty = true;
    EditorCommandPayload payload;
  };

  class EditorCommandHistory {
  public:
    void clear() noexcept;
    void record(EditorCommand command);

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] std::size_t count() const noexcept;
    [[nodiscard]] std::size_t applied_count() const noexcept;
    [[nodiscard]] bool can_undo() const noexcept;
    [[nodiscard]] bool can_redo() const noexcept;

    [[nodiscard]] const std::vector<EditorCommand>& commands() const noexcept;
    [[nodiscard]] const EditorCommand* last_command() const noexcept;

    [[nodiscard]] const EditorCommand* undo_command() noexcept;
    [[nodiscard]] const EditorCommand* redo_command() noexcept;

  private:
    std::vector<EditorCommand> m_commands;
    std::size_t m_next_command_index = 0;
  };

  [[nodiscard]] std::vector<GameObjectId>
  affected_ids(const EditorCommand& command);

  [[nodiscard]] EditorCommand make_object_change(std::string_view label,
                                                  const GameObject& before,
                                                  const GameObject& after,
                                                  std::string_view detail = {});

  [[nodiscard]] EditorCommand
  make_object_create(const GameObject& created, std::string_view detail = {});

  [[nodiscard]] EditorCommand
  make_object_delete(const GameObject& deleted, std::string_view detail = {});

  [[nodiscard]] EditorCommand
  make_objects_change(std::string_view label, std::span<const GameObject> before,
                      std::span<const GameObject> after,
                      std::string_view detail = {});

  [[nodiscard]] EditorCommand
  make_objects_delete(std::span<const GameObject> deleted,
                      std::string_view detail = {});

  [[nodiscard]] EditorCommand
  make_viewport_change(std::string_view label, const Camera& before,
                       const Camera& after, std::string_view detail = {});

} // namespace prune
