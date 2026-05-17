#pragma once

#include <string_view>

#include <SDL2/SDL.h>

#include "prune/core/input.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    class Scene {
    public:
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        virtual void on_enter() = 0;
        virtual void on_exit() = 0;
        virtual void update(float dt, const Input& input) = 0;
        virtual void render(SDL_Renderer* renderer) = 0;

        virtual void new_scene() = 0;

        [[nodiscard]] virtual bool save_to_file(std::string_view path, std::string& error) const = 0;
        [[nodiscard]] virtual bool load_from_file(std::string_view path, std::string& error) = 0;

        virtual void set_viewport(const SceneViewport& viewport) noexcept = 0;
        [[nodiscard]] virtual const SceneViewport& get_viewport() const noexcept = 0;
        [[nodiscard]] virtual int get_viewport_width() const noexcept = 0;
        [[nodiscard]] virtual int get_viewport_height() const noexcept = 0;

        [[nodiscard]] virtual std::string_view scene_name() const noexcept = 0;
        [[nodiscard]] virtual std::string_view scene_tools_label() const noexcept = 0;
        virtual void draw_scene_tools(bool& open) = 0;

        virtual GameObjectManager& get_object_manager() = 0;

        virtual GridOptions& get_grid_options() = 0;
        virtual SceneOptions& get_scene_options() = 0;

        virtual void draw_scene_inspector(GameObject& selected) = 0;

        virtual SceneCamera& get_camera() noexcept = 0;
        [[nodiscard]] virtual const SceneCamera& get_camera() const noexcept = 0;

    protected:
        Scene() = default;
    };
}
