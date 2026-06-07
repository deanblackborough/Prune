#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"

namespace prune {

    enum class EditorCommandType {
        MoveObject = 0,
        CreateObject,
        DeleteObject,
        RenameObject,
        ChangeObjectPosition,
        ChangeObjectSize,
        ChangeObjectRenderType,
        ChangeObjectColour,
        ChangeObjectFlag,
        ChangeSprite,
        MoveViewport
    };

    struct EditorCommand {
        EditorCommandType type = EditorCommandType::MoveObject;
        std::string label;
        GameObjectId object_id = k_invalid_game_object_id;

        std::optional<GameObject> before_object;
        std::optional<GameObject> after_object;

        std::optional<Camera> before_camera;
        std::optional<Camera> after_camera;
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

    [[nodiscard]] const char* editor_command_type_label(EditorCommandType type) noexcept;

    [[nodiscard]] EditorCommand make_object_command(
        EditorCommandType type,
        std::string label,
        const GameObject& before,
        const GameObject& after
    );

    [[nodiscard]] EditorCommand make_create_object_command(const GameObject& created);
    [[nodiscard]] EditorCommand make_delete_object_command(const GameObject& deleted);

    [[nodiscard]] EditorCommand make_viewport_command(
        std::string label,
        const Camera& before,
        const Camera& after
    );

}
