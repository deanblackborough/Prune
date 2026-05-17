#include "prune/scene/scene_shell.hpp"

namespace prune {

    SceneShell::SceneShell(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void SceneShell::on_enter()
    {
        new_scene();
    }

    void SceneShell::on_exit()
    {
        m_state.objects.clear();
        on_scene_exit();
    }

    void SceneShell::update(float dt, const Input& input)
    {
        update_scene(dt, input);
        m_interaction.update(m_state, dt, input);
        m_state.camera.update_game_camera(m_state.viewport, follow_target());
    }

    void SceneShell::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state);
        render_scene_overlay(renderer);
    }

    void SceneShell::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    GameObjectManager& SceneShell::get_object_manager()
    {
        return m_state.objects;
    }

    GridOptions& SceneShell::get_grid_options()
    {
        return m_state.grid_options;
    }

    SceneOptions& SceneShell::get_scene_options()
    {
        return m_state.scene_options;
    }

    SceneCamera& SceneShell::get_camera() noexcept
    {
        return m_state.camera;
    }

    const SceneCamera& SceneShell::get_camera() const noexcept
    {
        return m_state.camera;
    }

    void SceneShell::reset_common_state() noexcept
    {
        m_state.objects.clear();
        m_state.camera.reset();
        m_state.grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};
    }

    bool SceneShell::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    bool SceneShell::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }

}
