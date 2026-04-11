#pragma once

namespace prune {

    class SandboxScene;

    class EditorUI {
    public:
        void render(SandboxScene& scene);

    private:
        bool m_show_view_grid_options = false;
        bool m_show_outliner = true;
        bool m_show_inspector = true;
        bool m_show_controls = false;
        bool m_show_status = false;
        bool m_show_imgui_demo = false;
    };

} // namespace prune