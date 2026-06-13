#include "prune/editor/editor_command.hpp"

#include <utility>

namespace prune {

    void EditorCommandHistory::clear() noexcept
    {
        m_commands.clear();
        m_next_command_index = 0;
    }

    void EditorCommandHistory::record(EditorCommand command)
    {
        if (m_next_command_index < m_commands.size()) {
            m_commands.erase(m_commands.begin() + static_cast<std::ptrdiff_t>(m_next_command_index), m_commands.end());
        }

        m_commands.push_back(std::move(command));
        m_next_command_index = m_commands.size();
    }

    bool EditorCommandHistory::empty() const noexcept
    {
        return m_commands.empty();
    }

    std::size_t EditorCommandHistory::count() const noexcept
    {
        return m_commands.size();
    }

    std::size_t EditorCommandHistory::applied_count() const noexcept
    {
        return m_next_command_index;
    }

    bool EditorCommandHistory::can_undo() const noexcept
    {
        return m_next_command_index > 0;
    }

    bool EditorCommandHistory::can_redo() const noexcept
    {
        return m_next_command_index < m_commands.size();
    }

    const std::vector<EditorCommand>& EditorCommandHistory::commands() const noexcept
    {
        return m_commands;
    }

    const EditorCommand* EditorCommandHistory::last_command() const noexcept
    {
        if (m_next_command_index == 0 || m_commands.empty()) {
            return nullptr;
        }

        return &m_commands[m_next_command_index - 1];
    }

    const EditorCommand* EditorCommandHistory::undo_command() noexcept
    {
        if (!can_undo()) {
            return nullptr;
        }

        --m_next_command_index;
        return &m_commands[m_next_command_index];
    }

    const EditorCommand* EditorCommandHistory::redo_command() noexcept
    {
        if (!can_redo()) {
            return nullptr;
        }

        return &m_commands[m_next_command_index++];
    }

    const char* editor_command_type_label(EditorCommandType type) noexcept
    {
        switch (type) {
        case EditorCommandType::MoveObject:
            return "Move object";
        case EditorCommandType::CreateObject:
            return "Create object";
        case EditorCommandType::DeleteObject:
            return "Delete object";
        case EditorCommandType::RenameObject:
            return "Rename object";
        case EditorCommandType::ChangeObjectPosition:
            return "Change object position";
        case EditorCommandType::ChangeObjectSize:
            return "Change object size";
        case EditorCommandType::ChangeObjectRenderType:
            return "Change object rendering type";
        case EditorCommandType::ChangeObjectColour:
            return "Change object colour";
		case EditorCommandType::ChangeObjectFlag:
			return "Change object flag";
        case EditorCommandType::ChangeSprite:
            return "Change sprite";
        case EditorCommandType::MoveViewport:
            return "Move viewport";
        case EditorCommandType::MoveObjects:
            return "Move objects";
        case EditorCommandType::DeleteObjects:
            return "Delete objects";
        }

        return "Unknown editor command";
    }

    EditorCommand make_object_command(
        EditorCommandType type,
        std::string_view label,
        const GameObject& before,
        const GameObject& after,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = type;
        command.label = std::string(label);
        command.detail = std::string(detail);
        command.object_id = after.identity.id;
        command.object_ids.push_back(after.identity.id);
        command.before_object = before;
        command.after_object = after;

        return command;
    }

    EditorCommand make_create_object_command(
        const GameObject& created,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = EditorCommandType::CreateObject;
        command.label = editor_command_type_label(EditorCommandType::CreateObject);
        command.detail = std::string(detail);
        command.object_id = created.identity.id;
        command.object_ids.push_back(created.identity.id);
        command.after_object = created;

        return command;
    }

    EditorCommand make_delete_object_command(
        const GameObject& deleted,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = EditorCommandType::DeleteObject;
        command.label = editor_command_type_label(EditorCommandType::DeleteObject);
        command.detail = std::string(detail);
        command.object_id = deleted.identity.id;
        command.object_ids.push_back(deleted.identity.id);
        command.before_object = deleted;

        return command;
    }

    EditorCommand make_multi_object_command(
        EditorCommandType type,
        std::string_view label,
        std::span<const GameObject> before,
        std::span<const GameObject> after,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = type;
        command.label = std::string(label);
        command.detail = std::string(detail);
        command.before_objects.assign(before.begin(), before.end());
        command.after_objects.assign(after.begin(), after.end());

        command.object_ids.reserve(command.after_objects.size());
        for (const GameObject& object : command.after_objects) {
            command.object_ids.push_back(object.identity.id);
        }

        return command;
    }

    EditorCommand make_multi_delete_object_command(
        std::span<const GameObject> deleted,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = EditorCommandType::DeleteObjects;
        command.label = editor_command_type_label(EditorCommandType::DeleteObjects);
        command.detail = std::string(detail);
        command.before_objects.assign(deleted.begin(), deleted.end());

        command.object_ids.reserve(command.before_objects.size());
        for (const GameObject& object : command.before_objects) {
            command.object_ids.push_back(object.identity.id);
        }

        return command;
    }

    EditorCommand make_viewport_command(
        std::string_view label,
        const Camera& before,
        const Camera& after,
        std::string_view detail
    ) {
        EditorCommand command{};
        command.type = EditorCommandType::MoveViewport;
        command.label = std::string(label);
        command.detail = std::string(detail);
        command.before_camera = before;
        command.after_camera = after;

        return command;
    }

}
