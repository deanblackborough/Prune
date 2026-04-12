#pragma once

#include "prune/tooling/outliner.hpp"
#include "prune/tooling/inspector.hpp"

namespace prune {

    class SandboxScene;

    class EditorUI {
    public:
        void render(SandboxScene& scene);

    private:
        Outliner m_outliner;
        Inspector m_inspector;

        bool m_show_view_grid_options = false;
        bool m_show_outliner = true;
        bool m_show_inspector = true;
        bool m_show_controls = false;
        bool m_show_status = false;
        bool m_show_imgui_demo = false;
    };

} // namespace prune