#include "prune/scene/editor_scene.hpp"

namespace prune {

    EditorScene::EditorScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void EditorScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    GameObjectManager& EditorScene::get_object_manager()
    {
        return m_state.objects;
    }

    SceneOptions& EditorScene::get_scene_options()
    {
        return m_state.scene_options;
    }

    bool EditorScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    bool EditorScene::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }

    void EditorScene::on_enter()
    {
        new_scene();
    }

    void EditorScene::on_exit()
    {
        m_state.objects.clear();
        on_scene_exit();
    }

    void EditorScene::update(float dt, const Input& input)
    {
        update_scene(dt, input);
        m_interaction.update(m_state, get_camera(), get_grid_options(), dt, input);
        get_camera().update_game_camera(m_state.viewport, follow_target());
    }

    void EditorScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state, get_camera(), get_grid_options());
        render_scene_overlay(renderer);
    }

    void EditorScene::reset_common_state() noexcept
    {
        m_state.objects.clear();
        m_state.scene_options = {};
        m_state.drag_state = {};
    }

}
