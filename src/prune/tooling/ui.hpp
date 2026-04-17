#pragma once

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
        Outliner m_outliner;
        Inspector m_inspector;
        Controls m_controls;
        Stats m_stats;
        Options m_options;

        bool m_show_view_grid_options = false;
        bool m_show_outliner = true;
        bool m_show_inspector = true;
        bool m_show_controls = false;
        bool m_show_stats = false;
        bool m_show_imgui_demo = false;
    };

} // namespace prune