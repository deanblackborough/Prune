#include "prune/tooling/command_history.hpp"

#include <cstddef>

#include "imgui.h"

#include "prune/editor/editor_command.hpp"

namespace prune {

    void CommandHistoryPanel::draw(Scene& scene)
    {
        const EditorCommandHistory& history = scene.editor_command_history();

        ImGui::BeginDisabled(!history.can_undo());
        if (ImGui::Button("Undo")) {
            scene.undo_editor_command();
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        ImGui::BeginDisabled(!history.can_redo());
        if (ImGui::Button("Redo")) {
            scene.redo_editor_command();
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        ImGui::Text("Commands: %zu", history.count());

        if (history.empty()) {
            ImGui::TextUnformatted("No editor commands recorded.");
            return;
        }

        const std::size_t applied_count = history.applied_count();
        const auto& commands = history.commands();

        if (ImGui::BeginChild("##command_history_list", ImVec2(0.0f, 0.0f), true)) {
            for (std::size_t index = commands.size(); index > 0; --index) {
                const std::size_t command_index = index - 1;
                const EditorCommand& command = commands[command_index];
                const bool applied = command_index < applied_count;

                if (!applied) {
                    ImGui::BeginDisabled();
                }

                ImGui::Text(
                    "%s%zu. %s",
                    applied ? "" : "redo: ",
                    command_index + 1,
                    command.label.empty()
                        ? editor_command_type_label(command.type)
                        : command.label.c_str()
                );

                if (command.object_id != k_invalid_game_object_id) {
                    ImGui::SameLine();
                    ImGui::TextDisabled("#%u", command.object_id);
                }

                if (!applied) {
                    ImGui::EndDisabled();
                }
            }
        }
        ImGui::EndChild();
    }

}
