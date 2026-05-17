#pragma once

#include <string_view>

#include <SDL2/SDL.h>

#include "prune/scene/platformer_behaviour.hpp"
#include "prune/scene/platformer_state.hpp"
#include "prune/scene/scene_shell.hpp"
#include "prune/tooling/platformer.hpp"

namespace prune {

    class PlatformerScene : public SceneShell {
    public:
        PlatformerScene(int window_width, int window_height);

        void new_scene() override;

        [[nodiscard]] bool save_to_file(std::string_view path, std::string& error) const override;
        [[nodiscard]] bool load_from_file(std::string_view path, std::string& error) override;

        [[nodiscard]] std::string_view scene_name() const noexcept override { return "Platformer"; }
        [[nodiscard]] std::string_view scene_tools_label() const noexcept override { return "Platformer"; }
        void draw_scene_tools(bool& open) override;
        void draw_scene_inspector(GameObject& selected) override;

    protected:
        void update_scene(float dt, const Input& input) override;
        [[nodiscard]] GameObject* follow_target() noexcept override;
        void on_scene_exit() override;

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
