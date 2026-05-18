#pragma once

#include <SDL2/SDL.h>

#include "prune/scene/scene.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    // EditorScene is the shared abstract base for all concrete scene types.
    //
    // It owns only the universal editor plumbing — the state that every scene
    // needs regardless of genre (viewport, objects, scene options, drag state),
    // the shared renderer, and the shared interaction handler.
    //
    // Camera and grid options are intentionally NOT here. They are scene-specific:
    // not every scene type needs a scrolling game camera or a snap grid (e.g. a
    // card game scene). Concrete scenes own them directly and expose them through
    // the get_camera() / get_grid_options() pure virtuals inherited from Scene.
    //
    // Concrete scenes only implement what makes them different:
    //   - new_scene()            — populate default objects
    //   - save_to_file()         — scene-specific YAML data
    //   - load_from_file()       — scene-specific YAML data
    //   - scene_name()           — display identity
    //   - scene_tools_label()    — tool panel identity
    //   - draw_scene_tools()     — scene-specific tool UI
    //   - draw_scene_inspector() — scene-specific inspector UI
    //   - update_scene()         — runtime behaviour each frame
    //   - follow_target()        — object camera should follow (or nullptr)
    //   - get_camera()           — owns and returns its SceneCamera
    //   - get_grid_options()     — owns and returns its GridOptions
    //
    // Optional hooks with empty defaults:
    //   - on_scene_exit()             — reset scene-specific IDs on exit
    //   - render_scene_overlay()      — draw scene-specific overlays after render
    class EditorScene : public Scene {
    public:
        ~EditorScene() override = default;

        EditorScene(const EditorScene&) = delete;
        EditorScene& operator=(const EditorScene&) = delete;

        // Scene lifecycle — shared implementations call the virtual hooks below.
        void on_enter() override;
        void on_exit() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;

        // Viewport — all scenes store the viewport in m_state.
        void set_viewport(const SceneViewport& viewport) noexcept override;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept override { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept override { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept override { return m_state.viewport.height; }

        // Universal accessors — objects and options are owned here.
        GameObjectManager& get_object_manager() override;
        SceneOptions& get_scene_options() override;

    protected:
        EditorScene(int window_width, int window_height);

        // Concrete scenes implement their runtime behaviour each frame.
        virtual void update_scene(float dt, const Input& input) = 0;

        // Concrete scenes return the object the camera should follow (or nullptr).
        [[nodiscard]] virtual GameObject* follow_target() noexcept = 0;

        // Optional hook called from on_exit() after objects are cleared.
        // Override to reset scene-specific IDs or transient state.
        virtual void on_scene_exit() {}

        // Optional hook called from render() after the shared renderer runs.
        // Override to draw scene-specific overlays.
        virtual void render_scene_overlay(SDL_Renderer* renderer) { (void)renderer; }

        // Clears objects and resets universal editor state.
        // Concrete scenes call this from their reset method, then reset their
        // own camera and grid options.
        void reset_common_state() noexcept;

        // Input gating helpers based on viewport focus/hover state.
        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] bool scene_mouse_input_enabled() const noexcept;

        SceneState m_state;
        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
    };

}
