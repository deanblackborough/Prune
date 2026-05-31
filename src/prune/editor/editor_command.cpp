#include "prune/editor/editor_command.hpp"

#include <utility>

namespace prune {

    void EditorCommandHistory::clear() noexcept
    {
        m_commands.clear();
    }

    void EditorCommandHistory::record(EditorCommand command)
    {
        m_commands.push_back(std::move(command));
    }

    bool EditorCommandHistory::empty() const noexcept
    {
        return m_commands.empty();
    }

    std::size_t EditorCommandHistory::count() const noexcept
    {
        return m_commands.size();
    }

    const std::vector<EditorCommand>& EditorCommandHistory::commands() const noexcept
    {
        return m_commands;
    }

    const EditorCommand* EditorCommandHistory::last_command() const noexcept
    {
        if (m_commands.empty()) {
            return nullptr;
        }

        return &m_commands.back();
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
        case EditorCommandType::ChangeSprite:
            return "Change sprite";
        case EditorCommandType::MoveViewport:
            return "Move viewport";
        }

        return "Unknown editor command";
    }

    EditorCommand make_object_command(
        EditorCommandType type,
        std::string label,
        const GameObject& before,
        const GameObject& after
    ) {
        EditorCommand command{};
        command.type = type;
        command.label = std::move(label);
        command.object_id = after.identity.id;
        command.before_object = before;
        command.after_object = after;

        return command;
    }

    EditorCommand make_create_object_command(const GameObject& created)
    {
        EditorCommand command{};
        command.type = EditorCommandType::CreateObject;
        command.label = editor_command_type_label(EditorCommandType::CreateObject);
        command.object_id = created.identity.id;
        command.after_object = created;

        return command;
    }

    EditorCommand make_delete_object_command(const GameObject& deleted)
    {
        EditorCommand command{};
        command.type = EditorCommandType::DeleteObject;
        command.label = editor_command_type_label(EditorCommandType::DeleteObject);
        command.object_id = deleted.identity.id;
        command.before_object = deleted;

        return command;
    }

    EditorCommand make_viewport_command(
        std::string label,
        const Camera& before,
        const Camera& after
    ) {
        EditorCommand command{};
        command.type = EditorCommandType::MoveViewport;
        command.label = std::move(label);
        command.before_camera = before;
        command.after_camera = after;

        return command;
    }

}
