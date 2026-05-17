#pragma once

#include <string>

#include <SDL2/SDL.h>

#include "prune/tooling/outliner.hpp"
#include "prune/tooling/inspector.hpp"
#include "prune/tooling/controls.hpp"
#include "prune/tooling/stats.hpp"
#include "prune/tooling/options.hpp"

namespace prune {

    class Scene;

    class Ui {
    public:
        ~Ui();

        void build(Scene& scene, SDL_Renderer* renderer);
        void render_scene_viewport_content(Scene& scene, SDL_Renderer* renderer);

    private:
        void draw_scene_viewport(Scene& scene, SDL_Renderer* renderer);
        void draw_simple_shooter_panel(SimpleShooterScene& scene);

        void ensure_scene_render_target(SDL_Renderer* renderer, int width, int height);
        void destroy_scene_render_target();

        SDL_Texture* m_scene_render_target = nullptr;
        int m_scene_render_target_width = 0;
        int m_scene_render_target_height = 0;

        bool m_show_scene_viewport = true;

        static constexpr const char* kSceneFilePath = "simple_shooter_scene.yml";

        Outliner m_outliner;
        Inspector m_inspector;
        Controls m_controls;
        Stats m_stats;
        Options m_options;

        std::string m_file_status;
        bool m_file_status_is_error = false;

        bool m_show_view_grid_options = false;
        bool m_show_outliner = true;
        bool m_show_inspector = true;
        bool m_show_controls = false;
        bool m_show_stats = false;
        bool m_show_scene_tools = true;
        bool m_show_imgui_demo = false;
    };
}