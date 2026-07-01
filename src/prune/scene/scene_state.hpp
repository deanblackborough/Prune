#pragma once

#include "prune/core/defaults.hpp"
#include "prune/editor/editor_command.hpp"
#include "prune/editor/tools/transform_gizmo.hpp"
#include "prune/editor/editor_tool.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_event.hpp"

#include <vector>

namespace prune {

    struct SceneViewport {
        int screen_x = 0;
        int screen_y = 0;
        int width = 0;
        int height = 0;
        bool hovered = false;
        bool focused = false;
        bool keyboard_input_enabled = false;

        [[nodiscard]] bool has_area() const noexcept {
            return width > 0 && height > 0;
        }

        [[nodiscard]] bool contains(int x, int y) const noexcept {
            return x >= screen_x &&
                y >= screen_y &&
                x < (screen_x + width) &&
                y < (screen_y + height);
        }
    };

    struct DebugOverlayOptions {
        bool show_collision_bounds = false;
        bool show_runtime_markers = false;
        bool show_role_labels = false;
    };

    struct SceneOptions {
        bool highlight_selected = true;
        DebugOverlayOptions debug_overlays{};
    };

    struct GridOptions {
        bool show_grid = true;
        bool snap_to_grid = true;
        int grid_size = k_default_object_size;
        int nudge_step = 8;
        int shift_nudge_steps = 4;

        int min_grid_size = k_min_object_size;
        int max_grid_size = k_max_object_size;
        int min_nudge_step = 4;
        int max_nudge_step = 64;
    };

    enum class DragMode {
        None = 0,
        MoveObjects,
        ScaleObject
    };

    struct DragObjectStart {
        GameObjectId object_id = k_invalid_game_object_id;
        GameObject object{};
    };

    struct DragState {
        bool active = false;
        DragMode mode = DragMode::None;
        GameObjectId object_id = k_invalid_game_object_id;
        GameObject object_start{};
        std::vector<DragObjectStart> object_starts;
        Transform mouse_start_world{};
        editor::tools::transform_gizmo::ScaleHandle scale_handle =
            editor::tools::transform_gizmo::ScaleHandle::None;
    };

    struct SceneState {
        SceneViewport viewport{};
        DragState drag_state{};
        SceneOptions scene_options{};
        EditorTool editor_tool = EditorTool::Select;
        SceneEventQueue events;

        GameObjectManager objects;
        EditorCommandHistory editor_commands;
    };

}
