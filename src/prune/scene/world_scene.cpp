#include "prune/scene/world_scene.hpp"

namespace prune {

    void WorldScene::update(float dt, const Input& input)
    {
        update_runtime(dt, input, scene_keyboard_input_enabled());
        m_camera.update_game_camera(m_state.viewport, game_camera_target());
    }

    void WorldScene::update_editor(float dt, const Input& input)
    {
        m_interaction.update(m_state, m_camera, m_grid_options, dt, input);
        m_camera.update_game_camera(m_state.viewport, game_camera_target());
    }

    void WorldScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state, m_camera, m_grid_options);
        render_overlay(renderer);
    }

    void WorldScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    GameObjectManager& WorldScene::get_object_manager()
    {
        return m_state.objects;
    }

    SceneOptions& WorldScene::get_scene_options()
    {
        return m_state.scene_options;
    }

    WorldSceneContext WorldScene::world_scene_context() noexcept
    {
        return WorldSceneContext{ &m_grid_options, &m_camera };
    }

    ConstWorldSceneContext WorldScene::world_scene_context() const noexcept
    {
        return ConstWorldSceneContext{ &m_grid_options, &m_camera };
    }

    bool WorldScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    bool WorldScene::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }
}
