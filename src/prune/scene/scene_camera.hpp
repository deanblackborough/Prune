#pragma once

#include <SDL2/SDL.h>

#include "prune/scene/game_object.hpp"

namespace prune {

    struct SceneViewport;

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
        bool follow_target = true;
    };

    struct CameraState {
        Camera editor;
        Camera game;
        CameraMode mode = CameraMode::Editor;
        GameCameraOptions game_options;
    };

    class SceneCamera {
    public:
        SceneCamera() noexcept;

        [[nodiscard]] CameraState& state() noexcept;
        [[nodiscard]] const CameraState& state() const noexcept;

        [[nodiscard]] Camera& editor() noexcept;
        [[nodiscard]] const Camera& editor() const noexcept;

        [[nodiscard]] Camera& game() noexcept;
        [[nodiscard]] const Camera& game() const noexcept;

        [[nodiscard]] Camera& active() noexcept;
        [[nodiscard]] const Camera& active() const noexcept;

        void activate_editor() noexcept;
        void activate_game() noexcept;

        void reset() noexcept;

        void update_game_camera(const SceneViewport& viewport, const GameObject* target) noexcept;
        void pan_editor_by_mouse_delta(int delta_x, int delta_y) noexcept;

        [[nodiscard]] Transform screen_to_world(const SceneViewport& viewport, int screen_x, int screen_y) const noexcept;
        [[nodiscard]] SDL_Rect world_to_screen_rect(const GameObject& object) const noexcept;

    private:
        CameraState m_state;
    };
}