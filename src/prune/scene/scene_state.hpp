#pragma once

#include "prune/core/defaults.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune {

    struct SceneViewport {
        int screen_x = 0;
        int screen_y = 0;
        int width = 0;
        int height = 0;
        bool hovered = false;
        bool focused = false;

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

    struct SceneOptions {
        bool highlight_selected = true;
    };

    // GridOptions is scene-specific: not every scene type needs a grid
    // (e.g. a card game). Scenes that want grid support own this directly.
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

    // SceneState holds only the universal state that every scene type needs.
    // GridOptions, SceneCamera, and drag state are intentionally excluded:
    // they are scene-specific concerns owned by the things that need them.
    struct SceneState {
        SceneViewport viewport{};
        SceneOptions scene_options{};
        GameObjectManager objects;
    };

}
