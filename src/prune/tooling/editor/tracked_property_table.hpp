#pragma once

#include <optional>
#include <string>

#include "prune/editor/editor_command.hpp"
#include "prune/scene/game_object.hpp"

namespace prune {
    class Scene;
}

namespace prune::tooling::editor::tracked_property_table {

    class ObjectEditTracker {
    public:
        void capture_if_activated(const GameObject& before);
        void commit_if_deactivated_after_edit(Scene& scene, EditorCommandType type, const GameObject& after);
        void reset() noexcept;

    private:
        std::optional<GameObject> m_before;
    };

    bool drag_float(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        float& value,
        float speed = 1.0f,
        float min = 0.0f,
        float max = 0.0f,
        const char* format = "%.3f"
    );

    bool slider_int(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        int& value,
        int min,
        int max
    );

    bool color3(
        ObjectEditTracker& tracker,
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        float color[3]
    );

    bool checkbox(
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        bool& value
    );

    bool sprite_picker(
        Scene& scene,
        EditorCommandType type,
        GameObject& object,
        const char* label,
        const char* id,
        std::string& sprite_key
    );

    bool commit_if_changed(
        Scene& scene,
        EditorCommandType type,
        const GameObject& before,
        const GameObject& after
    );

}
