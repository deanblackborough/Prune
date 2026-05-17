
#include <algorithm>
#include <cmath>

#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    SceneCamera::SceneCamera() noexcept
    {
        reset();
    }

    CameraState& SceneCamera::state() noexcept
    {
        return m_state;
    }

    const CameraState& SceneCamera::state() const noexcept
    {
        return m_state;
    }

    Camera& SceneCamera::editor() noexcept
    {
        return m_state.editor;
    }

    const Camera& SceneCamera::editor() const noexcept
    {
        return m_state.editor;
    }

    Camera& SceneCamera::game() noexcept
    {
        return m_state.game;
    }

    const Camera& SceneCamera::game() const noexcept
    {
        return m_state.game;
    }

    Camera& SceneCamera::active() noexcept
    {
        return m_state.mode == CameraMode::Editor ? m_state.editor : m_state.game;
    }

    const Camera& SceneCamera::active() const noexcept
    {
        return m_state.mode == CameraMode::Editor ? m_state.editor : m_state.game;
    }

    void SceneCamera::activate_editor() noexcept
    {
        m_state.mode = CameraMode::Editor;
    }

    void SceneCamera::activate_game() noexcept
    {
        m_state.mode = CameraMode::Game;
    }

    void SceneCamera::reset() noexcept
    {
        m_state = {};

        m_state.editor.speed = 256.0f;
        m_state.editor.zoom = 1.0f;

        m_state.game.speed = 256.0f;
        m_state.game.zoom = 3.0f;

        m_state.mode = CameraMode::Editor;
    }

    void SceneCamera::update_game_camera(const SceneViewport& viewport, const GameObject* target) noexcept
    {
        if (!target || !m_state.game_options.follow_target || !viewport.has_area()) {
            return;
        }

        const float target_center_x =
            target->transform.x + (static_cast<float>(target->size.width) * 0.5f);

        const float target_center_y =
            target->transform.y + (static_cast<float>(target->size.height) * 0.5f);

        const float zoom = std::max(m_state.game.zoom, 0.01f);

        m_state.game.x = target_center_x - ((static_cast<float>(viewport.width) / zoom) * 0.5f);
        m_state.game.y = target_center_y - ((static_cast<float>(viewport.height) / zoom) * 0.5f);
    }

    void SceneCamera::pan_editor_by_mouse_delta(int delta_x, int delta_y) noexcept
    {
        const float zoom = std::max(m_state.editor.zoom, 0.01f);

        m_state.editor.x -= static_cast<float>(delta_x) / zoom;
        m_state.editor.y -= static_cast<float>(delta_y) / zoom;
    }

    Transform SceneCamera::screen_to_world(const SceneViewport& viewport, int screen_x, int screen_y) const noexcept
    {
        const Camera& camera = active();
        const float zoom = std::max(camera.zoom, 0.01f);

        const int local_x = screen_x - viewport.screen_x;
        const int local_y = screen_y - viewport.screen_y;

        return {
            camera.x + static_cast<float>(local_x) / zoom,
            camera.y + static_cast<float>(local_y) / zoom
        };
    }

    SDL_Rect SceneCamera::world_to_screen_rect(const GameObject& object) const noexcept
    {
        const Camera& camera = active();
        const float zoom = std::max(camera.zoom, 0.01f);

        return SDL_Rect{
            static_cast<int>(std::round((object.transform.x - camera.x) * zoom)),
            static_cast<int>(std::round((object.transform.y - camera.y) * zoom)),
            static_cast<int>(std::round(static_cast<float>(object.size.width) * zoom)),
            static_cast<int>(std::round(static_cast<float>(object.size.height) * zoom))
        };
    }
}