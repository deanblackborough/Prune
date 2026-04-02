#include "imgui_layer.hpp"

#include <SDL2/SDL.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

namespace prune {

    ImGuiLayer::ImGuiLayer(SDL_Window* window, SDL_GLContext gl_context)
        : m_show_demo(true) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
        ImGui_ImplOpenGL3_Init("#version 410");

        SDL_Log("ImGui layer initialized");
    }

    ImGuiLayer::~ImGuiLayer() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::process_event(const SDL_Event& event) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }

    void ImGuiLayer::begin_frame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiLayer::end_frame() {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiLayer::render_demo() {
        if (m_show_demo) {
            ImGui::ShowDemoWindow(&m_show_demo);
        }
    }

} // namespace prune