#pragma once

struct SDL_Window;
union SDL_Event;
typedef void* SDL_GLContext;

namespace prune {

    class ImGuiLayer {
    public:
        ImGuiLayer(SDL_Window* window, SDL_GLContext gl_context);
        ~ImGuiLayer();

        ImGuiLayer(const ImGuiLayer&) = delete;
        ImGuiLayer& operator=(const ImGuiLayer&) = delete;
        ImGuiLayer(ImGuiLayer&&) = delete;
        ImGuiLayer& operator=(ImGuiLayer&&) = delete;

        void process_event(const SDL_Event& event);

        void begin_frame();
        void end_frame();

        void render_demo();

    private:
        bool m_show_demo;
    };

} // namespace prune