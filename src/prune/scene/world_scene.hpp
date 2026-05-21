#pragma once

#include <SDL2/SDL.h>

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class WorldScene : public Scene {
    public:
        void update(float dt, const Input& input) final;
        void update_editor(float dt, const Input& input) final;
        void render(SDL_Renderer* renderer) final;

        void set_viewport(const SceneViewport& viewport) noexcept final;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept final { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept final { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept final { return m_state.viewport.height; }

        GameObjectManager& get_object_manager() final;
        SceneOptions& get_scene_options() final;

        [[nodiscard]] WorldSceneContext world_scene_context() noexcept final;
        [[nodiscard]] ConstWorldSceneContext world_scene_context() const noexcept final;

    protected:
        WorldScene() = default;

        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        virtual void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) = 0;
        [[nodiscard]] virtual GameObject* game_camera_target() noexcept { return nullptr; }
        virtual void render_overlay(SDL_Renderer*) {}

        SceneState m_state;
        GridOptions m_grid_options;
        SceneCamera m_camera;

    private:
        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
    };
}
