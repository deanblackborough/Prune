#include "prune/editor/editor_command.hpp"

#include <utility>

namespace prune {

  void EditorCommandHistory::clear() noexcept {
    m_commands.clear();
    m_next_command_index = 0;
  }

  void EditorCommandHistory::record(EditorCommand command) {
    if (m_next_command_index < m_commands.size()) {
      m_commands.erase(m_commands.begin() +
                           static_cast<std::ptrdiff_t>(m_next_command_index),
                       m_commands.end());
    }

    m_commands.push_back(std::move(command));
    m_next_command_index = m_commands.size();
  }

  bool EditorCommandHistory::empty() const noexcept {
    return m_commands.empty();
  }

  std::size_t EditorCommandHistory::count() const noexcept {
    return m_commands.size();
  }

  std::size_t EditorCommandHistory::applied_count() const noexcept {
    return m_next_command_index;
  }

  bool EditorCommandHistory::can_undo() const noexcept {
    return m_next_command_index > 0;
  }

  bool EditorCommandHistory::can_redo() const noexcept {
    return m_next_command_index < m_commands.size();
  }

  const std::vector<EditorCommand>&
  EditorCommandHistory::commands() const noexcept {
    return m_commands;
  }

  const EditorCommand* EditorCommandHistory::last_command() const noexcept {
    if (m_next_command_index == 0 || m_commands.empty()) {
      return nullptr;
    }

    return &m_commands[m_next_command_index - 1];
  }

  const EditorCommand* EditorCommandHistory::undo_command() noexcept {
    if (!can_undo()) {
      return nullptr;
    }

    --m_next_command_index;
    return &m_commands[m_next_command_index];
  }

  const EditorCommand* EditorCommandHistory::redo_command() noexcept {
    if (!can_redo()) {
      return nullptr;
    }

    return &m_commands[m_next_command_index++];
  }

  std::vector<GameObjectId> affected_ids(const EditorCommand& command) {
    return std::visit(
        [](const auto& payload) -> std::vector<GameObjectId> {
          using Payload = std::decay_t<decltype(payload)>;

          if constexpr (std::is_same_v<Payload, ObjectChange>) {
            return {payload.after.identity.id};
          } else if constexpr (std::is_same_v<Payload, ObjectCreate>) {
            return {payload.object.identity.id};
          } else if constexpr (std::is_same_v<Payload, ObjectDelete>) {
            return {payload.object.identity.id};
          } else if constexpr (std::is_same_v<Payload, ObjectsChange>) {
            std::vector<GameObjectId> ids;
            ids.reserve(payload.after.size());
            for (const GameObject& object : payload.after) {
              ids.push_back(object.identity.id);
            }
            return ids;
          } else if constexpr (std::is_same_v<Payload, ObjectsDelete>) {
            std::vector<GameObjectId> ids;
            ids.reserve(payload.objects.size());
            for (const GameObject& object : payload.objects) {
              ids.push_back(object.identity.id);
            }
            return ids;
          } else {
            static_assert(std::is_same_v<Payload, ViewportChange>);
            return {};
          }
        },
        command.payload);
  }

  EditorCommand make_object_change(std::string_view label,
                                   const GameObject& before,
                                   const GameObject& after,
                                   std::string_view detail) {
    EditorCommand command{};
    command.label = std::string(label);
    command.detail = std::string(detail);
    command.payload = ObjectChange{before, after};

    return command;
  }

  EditorCommand make_object_create(const GameObject& created,
                                   std::string_view detail) {
    EditorCommand command{};
    command.label = "Create object";
    command.detail = std::string(detail);
    command.payload = ObjectCreate{created};

    return command;
  }

  EditorCommand make_object_delete(const GameObject& deleted,
                                   std::string_view detail) {
    EditorCommand command{};
    command.label = "Delete object";
    command.detail = std::string(detail);
    command.payload = ObjectDelete{deleted};

    return command;
  }

  EditorCommand make_objects_change(std::string_view label,
                                    std::span<const GameObject> before,
                                    std::span<const GameObject> after,
                                    std::string_view detail) {
    EditorCommand command{};
    command.label = std::string(label);
    command.detail = std::string(detail);
    command.payload = ObjectsChange{
        std::vector<GameObject>(before.begin(), before.end()),
        std::vector<GameObject>(after.begin(), after.end())};

    return command;
  }

  EditorCommand make_objects_delete(std::span<const GameObject> deleted,
                                    std::string_view detail) {
    EditorCommand command{};
    command.label = "Delete objects";
    command.detail = std::string(detail);
    command.payload =
        ObjectsDelete{std::vector<GameObject>(deleted.begin(), deleted.end())};

    return command;
  }

  EditorCommand make_viewport_change(std::string_view label,
                                     const Camera& before, const Camera& after,
                                     std::string_view detail) {
    EditorCommand command{};
    command.makes_dirty = false;
    command.label = std::string(label);
    command.detail = std::string(detail);
    command.payload = ViewportChange{before, after};

    return command;
  }

} // namespace prune
