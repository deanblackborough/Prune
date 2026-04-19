#pragma once

#include <string>

#include "prune/tooling/outliner.hpp"
#include "prune/tooling/inspector.hpp"
#include "prune/tooling/controls.hpp"
#include "prune/tooling/stats.hpp"
#include "prune/tooling/options.hpp"

namespace prune {

    class SandboxScene;

    class Ui {
    public:
        void render(SandboxScene& scene);

    private:
        static constexpr const char* kSceneFilePath = "sandbox_scene.yml";

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
        bool m_show_imgui_demo = false;
    };
}