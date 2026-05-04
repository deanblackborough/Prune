#pragma once

#include "prune/core/defaults.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/player_controller.hpp"

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

    struct Camera {
        float x = 0.0f;
        float y = 0.0f;
        float speed = 256.0f;
        float zoom = 1.0f;
    };

    enum class CameraMode {
        Editor = 0,
        Game = 1
    };

    struct GameCameraOptions {
        bool follow_player = true;
    };

    struct CameraState {
        Camera editor;
        Camera game;
        CameraMode mode = CameraMode::Editor;
        GameCameraOptions game_options;
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

    struct DragState {
        bool active = false;
        GameObjectId object_id = k_invalid_game_object_id;
        Transform object_start{};
        Transform mouse_start_world{};
    };

    struct SimpleShooterOptions {
        float enemy_speed = 48.0f;
        float bullet_speed = 180.0f;
        float bullet_lifetime = 1.25f;
    };

    struct SceneState {
        GameObjectManager objects;
        PlayerController player_controller;

        GameObjectId player_id = k_invalid_game_object_id;
        GameObjectId enemy_id = k_invalid_game_object_id;

        SceneViewport viewport{};
        GridOptions grid_options{};
        SceneOptions scene_options{};
        DragState drag_state{};
        CameraState cameras{};
        SimpleShooterOptions simple_shooter_options{};
    };

}
