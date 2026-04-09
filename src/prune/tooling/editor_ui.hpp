#pragma once

namespace prune {

    class Scene;

    class EditorUI {
    public:
        void render(Scene& scene);

    private:
        bool m_show_viewport_panel = true;
        bool m_show_outline_panel = true;
        bool m_show_object_panel = true;
        bool m_show_debug = false;
        bool m_show_imgui_demo = false;
    };

} // namespace prune