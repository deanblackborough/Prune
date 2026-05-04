#include <algorithm>
#include <cmath>

#include <SDL2/SDL.h>
#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/core/input.hpp"

namespace prune {

    SandboxScene::SandboxScene(int window_width, int window_height)
    {
        m_state.viewport.width = window_width;
        m_state.viewport.height = window_height;
    }

    void SandboxScene::set_viewport(const SceneViewport& viewport) noexcept
    {
        m_state.viewport = viewport;
    }

    SceneState& SandboxScene::get_state() noexcept
    {
        return m_state;
    }

    const SceneState& SandboxScene::get_state() const noexcept
    {
        return m_state;
    }

    bool SandboxScene::scene_keyboard_input_enabled() const noexcept
    {
        return m_state.viewport.focused && m_state.viewport.has_area();
    }

    bool SandboxScene::scene_mouse_input_enabled() const noexcept
    {
        return m_state.viewport.hovered && m_state.viewport.has_area();
    }

    bool SandboxScene::scene_input_enabled() const noexcept
    {
        return scene_keyboard_input_enabled();
    }

    void SandboxScene::on_enter()
    {
        new_scene();
    }

    void SandboxScene::on_exit() {
        m_state.objects.clear();
        m_state.player_id = k_invalid_game_object_id;

        m_state.enemy_id = k_invalid_game_object_id;
    }

    void SandboxScene::render(SDL_Renderer* renderer)
    {
        m_renderer.render(renderer, m_state);
    }

    GameObject SandboxScene::create_player() {
        GameObject player;
        player.name = "Player";
        player.type = GameObjectType::Object;
        player.runtime.behaviour = "simple_shooter.player";
        player.editor.renameable = false;
        player.editor.movable = false;
        player.editor.deletable = false;
        player.editor.cloneable = false;
		player.size.width = k_default_object_size;
		player.size.height = k_default_object_size;
		player.render.type = RenderType::Sprite;
		player.render.sprite.sprite_key = std::string(k_default_player_sprite_key);
		player.render.rectangle.color[0] = 0.3f;
		player.render.rectangle.color[1] = 0.8f;
		player.render.rectangle.color[2] = 0.5f;
        player.transform.x = 128.0f;
        player.transform.y = 128.0f;
        player.active = true;
        player.render.visible = true;
        player.collision.solid = false;

        return player;
    }

    GameObject SandboxScene::create_initial_block() {
        GameObject block;
        block.name = "Static Block";
        block.type = GameObjectType::Object;
        block.runtime.behaviour = "";
        block.transform.x = 176.0f;
        block.transform.y = 128.0f;
        block.size.width = k_default_object_size;
        block.size.height = k_default_object_size;
        block.render.type = RenderType::Rectangle;
        block.render.rectangle.color[0] = 0.8f;
        block.render.rectangle.color[1] = 0.5f;
        block.render.rectangle.color[2] = 0.2f;
        block.active = true;
        block.collision.solid = true;

        return block;
    }

    GameObject SandboxScene::create_enemy()
    {
        GameObject enemy;
        enemy.name = "Enemy";
        enemy.type = GameObjectType::Object;
        enemy.runtime.behaviour = "simple_shooter.enemy";
        enemy.transform.x = 256.0f;
        enemy.transform.y = 128.0f;
        enemy.size.width = k_default_object_size;
        enemy.size.height = k_default_object_size;
        enemy.render.type = RenderType::Sprite;
        enemy.render.sprite.sprite_key = "tank-red";
        enemy.render.rectangle.color[0] = 0.9f;
        enemy.render.rectangle.color[1] = 0.2f;
        enemy.render.rectangle.color[2] = 0.2f;
        enemy.active = true;
        enemy.render.visible = true;
        enemy.collision.solid = false;

        return enemy;
    }

    GameObjectManager& SandboxScene::get_object_manager() {
        return m_state.objects;
    }

    GameObjectId SandboxScene::get_player_id() const {
        return m_state.player_id;
    }

    PlayerController & SandboxScene::get_player_controller() {
        return m_state.player_controller;
    }

    GridOptions& SandboxScene::get_grid_options() {
        return m_state.grid_options;
    }

    SceneOptions& SandboxScene::get_scene_options() {
        return m_state.scene_options;
    }


    CameraState& SandboxScene::get_camera_state() noexcept {
        return m_state.cameras;
    }

    const CameraState& SandboxScene::get_camera_state() const noexcept {
        return m_state.cameras;
    }

    Camera& SandboxScene::get_editor_camera() noexcept {
        return m_state.cameras.editor;
    }

    const Camera& SandboxScene::get_editor_camera() const noexcept {
        return m_state.cameras.editor;
    }

    Camera& SandboxScene::get_game_camera() noexcept {
        return m_state.cameras.game;
    }

    const Camera& SandboxScene::get_game_camera() const noexcept {
        return m_state.cameras.game;
    }

    Camera& SandboxScene::get_active_camera() noexcept {
        return (m_state.cameras.mode == CameraMode::Editor) ? m_state.cameras.editor : m_state.cameras.game;
    }

    const Camera& SandboxScene::get_active_camera() const noexcept {
        return (m_state.cameras.mode == CameraMode::Editor) ? m_state.cameras.editor : m_state.cameras.game;
    }

    void SandboxScene::activate_editor_camera() noexcept
    {
        m_state.cameras.mode = CameraMode::Editor;
    }

    void SandboxScene::activate_game_camera() noexcept
    {
        m_state.cameras.mode = CameraMode::Game;
    }

    SimpleShooterOptions& SandboxScene::get_simple_shooter_options() noexcept
    {
        return m_state.simple_shooter_options;
    }

    const SimpleShooterOptions& SandboxScene::get_simple_shooter_options() const noexcept
    {
        return m_state.simple_shooter_options;
    }

    GameObject* SandboxScene::enemy_object() noexcept
    {
        return m_state.objects.get_by_id(m_state.enemy_id);
    }

    const GameObject* SandboxScene::enemy_object() const noexcept
    {
        return m_state.objects.get_by_id(m_state.enemy_id);
    }

    int SandboxScene::bullet_count() const noexcept
    {
        int count = 0;

        for (const auto& object : m_state.objects.objects()) {
            if (object.runtime.behaviour == "simple_shooter.bullet" && object.active) {
                ++count;
            }
        }

        return count;
    }

    void SandboxScene::reset_simple_shooter()
    {
        for (auto& object : m_state.objects.objects()) {
            if (object.runtime.behaviour == "simple_shooter.bullet") {
                object.active = false;
            }
        }

        cleanup_runtime_objects();

        GameObject* enemy = enemy_object();
        if (!enemy) {
            m_state.enemy_id = m_state.objects.create_object(create_enemy());
            return;
        }

		respawn_enemy(*enemy);
    }

    void SandboxScene::update(float dt, const Input& input)
    {
        update_game(dt, input);
        m_interaction.update(m_state, dt, input);
		update_cameras();
    }

    void SandboxScene::update_game(float dt, const Input& input)
    {
        update_player(dt, input);
        handle_player_shooting(input);
        update_enemy(dt);
        update_bullets(dt);
        handle_bullet_enemy_collisions();
        cleanup_runtime_objects();
    }

    void SandboxScene::update_player(float dt, const Input& input)
    {
        GameObject* player = player_object();
        if (!player) {
            return;
        }

        if (!scene_keyboard_input_enabled()) {
            player->velocity = {};
            return;
        }

        player->velocity = m_state.player_controller.movement_velocity(input);

        const bool is_moving =
            player->velocity.x != 0.0f ||
            player->velocity.y != 0.0f;

        if (is_moving) {
            activate_game_camera();
            update_player_facing(*player);
        }

        move_object(*player, player->velocity.x * dt, player->velocity.y * dt, true);
    }

    void SandboxScene::update_player_facing(GameObject& player) noexcept
    {
        if (std::abs(player.velocity.x) > std::abs(player.velocity.y)) {
            player.facing = player.velocity.x < 0.0f ? Direction::Left : Direction::Right;
            return;
        }

        if (player.velocity.y != 0.0f) {
            player.facing = player.velocity.y < 0.0f ? Direction::Up : Direction::Down;
        }
    }

    void SandboxScene::handle_player_shooting(const Input& input)
    {
        if (!scene_keyboard_input_enabled()) {
            return;
        }

        if (!input.was_key_pressed(SDL_SCANCODE_SPACE)) {
            return;
        }

        const GameObject* player = player_object();
        if (!player) {
            return;
        }

        create_bullet_from_player(*player);
    }

    void SandboxScene::create_bullet_from_player(const GameObject& player)
    {
        GameObject bullet;
        bullet.name = "Bullet";
        bullet.type = GameObjectType::Runtime;
        bullet.runtime.behaviour = "simple_shooter.bullet";
        bullet.runtime.persistent = false;

        bullet.editor.selectable = false;
        bullet.editor.renameable = false;
        bullet.editor.movable = false;
        bullet.editor.deletable = false;
        bullet.editor.cloneable = false;

        bullet.size.width = 4;
        bullet.size.height = 4;
        bullet.render.type = RenderType::Rectangle;
        bullet.render.rectangle.color[0] = 0.95f;
        bullet.render.rectangle.color[1] = 0.9f;
        bullet.render.rectangle.color[2] = 0.35f;
        bullet.collision.solid = false;
        bullet.active = true;
        bullet.render.visible = true;
        bullet.facing = player.facing;
        bullet.lifetime = m_state.simple_shooter_options.bullet_lifetime;

        const float player_center_x = player.transform.x + (static_cast<float>(player.size.width) * 0.5f);
        const float player_center_y = player.transform.y + (static_cast<float>(player.size.height) * 0.5f);

        bullet.transform.x = player_center_x - (static_cast<float>(bullet.size.width) * 0.5f);
        bullet.transform.y = player_center_y - (static_cast<float>(bullet.size.height) * 0.5f);

        switch (player.facing) {
        case Direction::Up:
            bullet.velocity.y = -m_state.simple_shooter_options.bullet_speed;
            break;
        case Direction::Down:
            bullet.velocity.y = m_state.simple_shooter_options.bullet_speed;
            break;
        case Direction::Left:
            bullet.velocity.x = -m_state.simple_shooter_options.bullet_speed;
            break;
        case Direction::Right:
            bullet.velocity.x = m_state.simple_shooter_options.bullet_speed;
            break;
        }

        m_state.objects.create_object(bullet);
    }

    void SandboxScene::update_enemy(float dt)
    {
        GameObject* enemy = enemy_object();
        const GameObject* player = player_object();

        if (!enemy || !enemy->active || !player) {
            return;
        }

        const float enemy_center_x = enemy->transform.x + (static_cast<float>(enemy->size.width) * 0.5f);
        const float enemy_center_y = enemy->transform.y + (static_cast<float>(enemy->size.height) * 0.5f);
        const float player_center_x = player->transform.x + (static_cast<float>(player->size.width) * 0.5f);
        const float player_center_y = player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

        float direction_x = player_center_x - enemy_center_x;
        float direction_y = player_center_y - enemy_center_y;
        const float length = std::sqrt((direction_x * direction_x) + (direction_y * direction_y));

        if (length <= 0.001f) {
            enemy->velocity = {};
            return;
        }

        direction_x /= length;
        direction_y /= length;

        enemy->velocity.x = direction_x * m_state.simple_shooter_options.enemy_speed;
        enemy->velocity.y = direction_y * m_state.simple_shooter_options.enemy_speed;

        move_object(*enemy, enemy->velocity.x * dt, enemy->velocity.y * dt, false);
    }

    void SandboxScene::update_bullets(float dt)
    {
        for (auto& object : m_state.objects.objects()) {
            if (object.runtime.behaviour != "simple_shooter.bullet" || !object.active) {
                continue;
            }

            object.transform.x += object.velocity.x * dt;
            object.transform.y += object.velocity.y * dt;
            object.lifetime -= dt;

            if (object.lifetime <= 0.0f) {
                object.active = false;
            }
        }
    }

    void SandboxScene::handle_bullet_enemy_collisions()
    {
        GameObject* enemy = enemy_object();
        if (!enemy || !enemy->active) {
            return;
        }

        for (auto& object : m_state.objects.objects()) {
            if (object.runtime.behaviour != "simple_shooter.bullet" || !object.active) {
                continue;
            }

            if (!is_overlapping(object, *enemy)) {
                continue;
            }

            object.active = false;

			respawn_enemy(*enemy);

            return;
        }
    }

    void SandboxScene::respawn_enemy(GameObject& enemy)
    {
        enemy.transform.x = 256.0f;
        enemy.transform.y = 128.0f;
        enemy.velocity = {};
        enemy.active = true;
        enemy.render.visible = true;
    }

    void SandboxScene::cleanup_runtime_objects()
    {
        auto& objects = m_state.objects.objects();

        objects.erase(
            std::remove_if(
                objects.begin(),
                objects.end(),
                [](const GameObject& object) {
                    return object.runtime.behaviour == "simple_shooter.bullet" && !object.active;
                }
            ),
            objects.end()
        );
    }

    void SandboxScene::update_cameras()
    {
        update_game_camera();
    }

    void SandboxScene::update_game_camera() noexcept
    {
        const GameObject* player = player_object();
        if (!player) {
            return;
        }

        if (!m_state.cameras.game_options.follow_player) {
            return;
        }

        const float player_center_x =
            player->transform.x + (static_cast<float>(player->size.width) * 0.5f);

        const float player_center_y =
            player->transform.y + (static_cast<float>(player->size.height) * 0.5f);

        if (!m_state.viewport.has_area()) {
            return;
        }

        const float zoom = std::max(m_state.cameras.game.zoom, 0.01f);

        m_state.cameras.game.x = player_center_x - ((static_cast<float>(m_state.viewport.width) / zoom) * 0.5f);
        m_state.cameras.game.y = player_center_y - ((static_cast<float>(m_state.viewport.height) / zoom) * 0.5f);
    }

    void SandboxScene::move_object(GameObject& object, float delta_x, float delta_y, bool resolve_collisions)
    {
        object.transform.x += delta_x;
        object.transform.y += delta_y;

        if (resolve_collisions && object.id == m_state.player_id) {
            resolve_player_collisions(object);
        }
    }

    GameObject* SandboxScene::player_object() noexcept
    {
        return m_state.objects.get_by_id(m_state.player_id);
    }

    const GameObject* SandboxScene::player_object() const noexcept
    {
        return m_state.objects.get_by_id(m_state.player_id);
    }

    bool SandboxScene::is_overlapping(const GameObject& a, const GameObject& b) const noexcept
    {
        const RectF a_bounds = a.bounds();
        const RectF b_bounds = b.bounds();

        return a_bounds.x < (b_bounds.x + b_bounds.width) &&
               (a_bounds.x + a_bounds.width) > b_bounds.x &&
               a_bounds.y < (b_bounds.y + b_bounds.height) &&
               (a_bounds.y + a_bounds.height) > b_bounds.y;
    }

    void SandboxScene::resolve_player_collisions(GameObject& player)
    {
        for (const auto& object : m_state.objects.objects()) {
            if (object.id == player.id || !object.active || !object.collision.solid) {
                continue;
            }

            if (!is_overlapping(player, object)) {
                continue;
            }

            const RectF player_bounds = player.bounds();
            const RectF block_bounds = object.bounds();

            const float overlap_left = (player_bounds.x + player_bounds.width) - block_bounds.x;
            const float overlap_right = (block_bounds.x + block_bounds.width) - player_bounds.x;
            const float overlap_top = (player_bounds.y + player_bounds.height) - block_bounds.y;
            const float overlap_bottom = (block_bounds.y + block_bounds.height) - player_bounds.y;

            const float resolve_x = (overlap_left < overlap_right) ? -overlap_left : overlap_right;
            const float resolve_y = (overlap_top < overlap_bottom) ? -overlap_top : overlap_bottom;

            if (std::abs(resolve_x) < std::abs(resolve_y)) {
                player.transform.x += resolve_x;
                player.velocity.x = 0.0f;
            } else {
                player.transform.y += resolve_y;
                player.velocity.y = 0.0f;
            }
        }
    }

    void SandboxScene::reset_runtime_state()
    {
        m_state.objects.clear();
        m_state.player_id = k_invalid_game_object_id;

        m_state.cameras = {};
        m_state.cameras.editor.speed = 256.0f;
        m_state.cameras.editor.zoom = 1.0f;

        m_state.cameras.game.speed = 256.0F;
        m_state.cameras.game.zoom = 3.0f;

        m_state.cameras.mode = CameraMode::Editor;
        m_state.cameras.game_options.follow_player = true;

        m_state.grid_options = {};
        m_state.scene_options = {};
        m_state.drag_state = {};
        m_state.player_controller = {};

        m_state.enemy_id = k_invalid_game_object_id;
        m_state.simple_shooter_options = {};
    }

    void SandboxScene::restore_defaults()
    {
        reset_runtime_state();

        m_state.player_id = m_state.objects.create_object(create_player());
        m_state.objects.create_object(create_initial_block());
        m_state.enemy_id = m_state.objects.create_object(create_enemy());
        m_state.objects.select(m_state.player_id);

		update_game_camera();
    }

    void SandboxScene::new_scene()
    {
        restore_defaults();

        update_game_camera();
    }
}
