#pragma once

#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/platformer_behaviour.hpp"
#include "prune/scene/platformer_state.hpp"
#include "prune/scene/world_scene.hpp"
#include "prune/tooling/platformer.hpp"

namespace prune {

    class PlatformerScene : public WorldScene {
    public:
        PlatformerScene(int window_width, int window_height);

        void on_enter() override;
        void on_exit() override;

        void new_scene() override;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Platformer"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Platformer"; }
        void draw_scene_tools(bool& open) override;

        void draw_scene_inspector(GameObject& selected) override;

    protected:
        void update_runtime(float dt, const Input& input, bool keyboard_input_enabled) override;
        [[nodiscard]] GameObject* game_camera_target() noexcept override;

    private:
        void reset_runtime_state();
        void restore_defaults();
        [[nodiscard]] GameObject* player_object() noexcept;
        [[nodiscard]] const GameObject* player_object() const noexcept;

        void add_platform_at_view_center();
        void add_hazard_at_view_center();
        [[nodiscard]] Transform view_center_spawn_position(int width, int height) const;

        PlatformerState m_platformer_state;
        PlatformerBehaviour m_platformer;
        Platformer m_platformer_tools;
    };

}
