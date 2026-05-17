#pragma once

#include <SDL2/SDL.h>

#include "prune/scene/scene.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    // SceneShell is the shared base for all concrete scene types.
    //
    // It owns the common editor/runtime plumbing (state, renderer, interaction)
    // and implements all Scene interface methods that are identical across scenes.
    // Concrete scenes inherit from SceneShell and implement only what makes them different:
    // update_scene, follow_target, new_scene, save/load, name, tools, and inspector.
    class SceneShell : public Scene {
    public:
        ~SceneShell() override = default;

        SceneShell(const SceneShell&) = delete;
        SceneShell& operator=(const SceneShell&) = delete;

        // Scene lifecycle — shared implementation calls the virtual hooks below.
        void on_enter() override;
        void on_exit() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;

        // Viewport — all scenes store the viewport in m_state.
        void set_viewport(const SceneViewport& viewport) noexcept override;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept override { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept override { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept override { return m_state.viewport.height; }

        // Common accessors — all scenes expose the same shared state.
        GameObjectManager& get_object_manager() override;
        GridOptions& get_grid_options() override;
        SceneOptions& get_scene_options() override;
        SceneCamera& get_camera() noexcept override;
        [[nodiscard]] const SceneCamera& get_camera() const noexcept override;

    protected:
        SceneShell(int window_width, int window_height);

        // Concrete scenes implement their runtime update here.
        virtual void update_scene(float dt, const Input& input) = 0;

        // Concrete scenes return the object the game camera should follow (or nullptr).
        [[nodiscard]] virtual GameObject* follow_target() noexcept = 0;

        // Optional hook called from on_exit() after objects are cleared.
        // Override to reset scene-specific IDs or transient state.
        virtual void on_scene_exit() {}

        // Optional hook called from render() after the shared renderer runs.
        // Override to draw scene-specific overlays.
        virtual void render_scene_overlay(SDL_Renderer* renderer) { (void)renderer; }

        // Resets the common shared state: clears objects, resets camera,
        // grid options, scene options, and drag state.
        void reset_common_state() noexcept;

        // Input gating helpers based on viewport focus/hover state.
        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        SceneState m_state;
        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
    };

}
