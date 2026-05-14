#include <SDL2/SDL.h>

#include "prune/core/input.hpp"
#include "prune/scene/simple_shooter_scene.hpp"
#include "prune/scene/scene_serializer.hpp"
#include "prune/scene/simple_shooter_factory.hpp"

namespace prune {

    SimpleShooterScene::SimpleShooterScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void SimpleShooterScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    SceneState& SimpleShooterScene::get_state() noexcept
    {
        return m_state;
    }

    const SceneState& SimpleShooterScene::get_state() const noexcept
    {
        return m_state;
    }

    bool SimpleShooterScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    bool SimpleShooterScene::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }

    void SimpleShooterScene::on_enter()
    {
        new_scene();
    }

    void SimpleShooterScene::on_exit() {
        m_state.objects.clear();
        m_state.player_id = k_invalid_game_object_id;

        m_state.enemy_id = k_invalid_game_object_id;
    }

    void SimpleShooterScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state);
    }

    GameObjectManager& SimpleShooterScene::get_object_manager() {
        return m_state.objects;
    }

    GameObjectId SimpleShooterScene::get_player_id() const {
        return m_state.player_id;
    }

    PlayerController & SimpleShooterScene::get_player_controller() {
        return m_state.player_controller;
    }

    GridOptions& SimpleShooterScene::get_grid_options() {
        return m_state.grid_options;
    }

    SceneOptions& SimpleShooterScene::get_scene_options() {
        return m_state.scene_options;
    }

    SceneCamera& SimpleShooterScene::get_camera() noexcept
    {
        return m_state.camera;
    }

    const SceneCamera& SimpleShooterScene::get_camera() const noexcept
    {
        return m_state.camera;
    }

    SimpleShooterOptions& SimpleShooterScene::get_simple_shooter_options() noexcept
    {
        return m_state.simple_shooter_options;
    }

    const SimpleShooterOptions& SimpleShooterScene::get_simple_shooter_options() const noexcept
    {
        return m_state.simple_shooter_options;
    }

    GameObject* SimpleShooterScene::enemy_object() noexcept
    {
        return m_simple_shooter.enemy_object(m_state);
    }

    const GameObject* SimpleShooterScene::enemy_object() const noexcept
    {
        return m_simple_shooter.enemy_object(m_state);
    }

    int SimpleShooterScene::bullet_count() const noexcept
    {
        return m_simple_shooter.bullet_count(m_state);
    }

    void SimpleShooterScene::reset_simple_shooter()
    {
        m_simple_shooter.reset(m_state);
    }

    void SimpleShooterScene::update(float dt, const Input& input)
    {
        m_simple_shooter.update(m_state, dt, input, scene_keyboard_input_enabled());
        m_interaction.update(m_state, dt, input);
        m_state.camera.update_game_camera(m_state.viewport, player_object());
    }

    GameObject* SimpleShooterScene::player_object() noexcept
    {
        return m_state.objects.get_by_id(m_state.player_id);
    }

    const GameObject* SimpleShooterScene::player_object() const noexcept
    {
        return m_state.objects.get_by_id(m_state.player_id);
    }

    void SimpleShooterScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_state.player_id = k_invalid_game_object_id;

        m_state.camera.reset();

        m_state.grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};
        m_state.player_controller = {};

        m_state.enemy_id = k_invalid_game_object_id;
        m_state.simple_shooter_options = {};
    }

    void SimpleShooterScene::restore_defaults()
    {
        reset_runtime_state();

        m_state.player_id = m_state.objects.create_object(simple_shooter_factory::create_player());
        m_state.objects.create_object(simple_shooter_factory::create_initial_block());
        m_state.enemy_id = m_state.objects.create_object(simple_shooter_factory::create_enemy());
        m_state.objects.select(m_state.player_id);

        m_state.camera.update_game_camera(m_state.viewport, player_object());
    }

    void SimpleShooterScene::new_scene()
    {
        restore_defaults();

        m_state.camera.update_game_camera(m_state.viewport, player_object());
    }

    bool SimpleShooterScene::save_to_file(std::string_view path, std::string& error) const
    {
        return SceneSerializer::save_to_file(m_state, path, error);
    }

    bool SimpleShooterScene::load_from_file(std::string_view path, std::string& error)
    {
        return SceneSerializer::load_from_file(m_state, path, error);
    }
}
