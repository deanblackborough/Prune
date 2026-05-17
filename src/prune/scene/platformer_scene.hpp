#pragma once

#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/platformer_behaviour.hpp"
#include "prune/scene/platformer_state.hpp"
#include "prune/scene/scene.hpp"
#include "prune/scene/scene_interaction.hpp"
#include "prune/scene/scene_renderer.hpp"
#include "prune/tooling/platformer.hpp"

namespace prune {

    class PlatformerScene : public Scene {
    public:
        PlatformerScene(int window_width, int window_height);

        void on_enter() override;
        void on_exit() override;
        void update(float dt, const Input& input) override;
        void render(SDL_Renderer* renderer) override;

        void new_scene() override;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;

        void set_viewport(const SceneViewport& viewport) noexcept override;
        [[nodiscard]] const SceneViewport& get_viewport() const noexcept override { return m_state.viewport; }
        [[nodiscard]] int get_viewport_width() const noexcept override { return m_state.viewport.width; }
        [[nodiscard]] int get_viewport_height() const noexcept override { return m_state.viewport.height; }

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Platformer"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Platformer"; }
        void draw_scene_tools(bool& open) override;

        GameObjectManager& get_object_manager() override;
        GridOptions& get_grid_options() override;
        SceneOptions& get_scene_options() override;
        void draw_scene_inspector(GameObject& selected) override;

        SceneCamera& get_camera() noexcept override;
        [[nodiscard]] const SceneCamera& get_camera() const noexcept override;

    private:
        void reset_runtime_state();
        void restore_defaults();
        [[nodiscard]] bool scene_keyboard_input_enabled() const noexcept;
        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        void add_platform_at_view_center();
        [[nodiscard]] Transform view_center_spawn_position(int width, int height) const;

        SceneState m_state;
        PlatformerState m_platformer_state;
        SceneRenderer m_renderer;
        SceneInteraction m_interaction;
        PlatformerBehaviour m_platformer;
        Platformer m_platformer_tools;
    };

}
