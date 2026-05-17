#include <utility>

#include "imgui.h"

#include "prune/core/defaults.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    Ui::~Ui()
    {
        destroy_scene_render_target();
    }

    void Ui::build(
        Scene& scene,
        SDL_Renderer* renderer,
        bool& new_scene_requested,
        bool& load_scene_requested
    )
    {
        draw_scene_viewport(scene, renderer);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    new_scene_requested = true;
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene")) {
                    std::string error;
                    if (scene.save_to_file(k_default_scene_file_path, error)) {
                        m_file_status = "Saved scene to " + std::string(k_default_scene_file_path);
                        m_file_status_is_error = false;
                    }
                    else {
                        m_file_status = "Save failed: " + error;
                        m_file_status_is_error = true;
                    }
                }

                if (ImGui::MenuItem("Load Scene")) {
                    load_scene_requested = true;
                }

                ImGui::Separator();
                ImGui::MenuItem("Options", nullptr, &m_show_view_grid_options);

                if (!m_file_status.empty()) {
                    ImGui::Separator();
                    if (m_file_status_is_error) {
                        ImGui::TextWrapped("Status: %s", m_file_status.c_str());
                    }
                    else {
                        ImGui::TextWrapped("Status: %s", m_file_status.c_str());
                    }
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Scene", nullptr, &m_show_scene_viewport);
                ImGui::MenuItem("Outliner", nullptr, &m_show_outliner);
                ImGui::MenuItem("Inspector", nullptr, &m_show_inspector);
                ImGui::MenuItem(scene.scene_tools_label().data(), nullptr, &m_show_scene_tools);
                ImGui::MenuItem("Stats", nullptr, &m_show_stats);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help")) {
                ImGui::MenuItem("Controls", nullptr, &m_show_controls);
                ImGui::MenuItem("ImGui Demo", nullptr, &m_show_imgui_demo);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (m_show_view_grid_options) {
            tooling::EditorLayout::options();

            if (ImGui::Begin("Options", &m_show_view_grid_options)) {
                m_options.draw(scene.get_scene_options(), scene.get_grid_options(), scene.get_camera());
            }
            ImGui::End();
        }

        if (m_show_outliner) {
            tooling::EditorLayout::outliner();

            if (ImGui::Begin("Outliner", &m_show_outliner)) {
                m_outliner.draw(scene.get_object_manager());
            }
            ImGui::End();
        }

        if (m_show_inspector) {
            tooling::EditorLayout::inspector();

            if (ImGui::Begin("Inspector", &m_show_inspector)) {
                m_inspector.draw(
                    scene.get_object_manager(),
                    scene.get_grid_options(),
                    scene.get_camera().active()
                );
            }
            ImGui::End();
        }

        if (m_show_scene_tools) {
            scene.draw_scene_tools(m_show_scene_tools);
        }

        if (m_show_controls) {
            tooling::EditorLayout::controls();

            if (ImGui::Begin("Controls", &m_show_controls)) {
                m_controls.draw();
            }
            ImGui::End();
        }

        if (m_show_stats) {
            tooling::EditorLayout::stats();

            if (ImGui::Begin("Stats", &m_show_stats)) {
                m_stats.draw(
                    scene.get_object_manager(),
                    scene.get_viewport(),
                    scene.get_camera()
                );
            }
            ImGui::End();
        }

        if (m_show_imgui_demo) {
            ImGui::ShowDemoWindow(&m_show_imgui_demo);
        }
    }

    void Ui::destroy_scene_render_target()
    {
        if (m_scene_render_target) {
            SDL_DestroyTexture(m_scene_render_target);
            m_scene_render_target = nullptr;
        }

        m_scene_render_target_width = 0;
        m_scene_render_target_height = 0;
    }

    void Ui::ensure_scene_render_target(SDL_Renderer* renderer, int width, int height)
    {
        width = std::max(1, width);
        height = std::max(1, height);

        if (m_scene_render_target &&
            m_scene_render_target_width == width &&
            m_scene_render_target_height == height) {
            return;
        }

        destroy_scene_render_target();

        m_scene_render_target = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA8888,
            SDL_TEXTUREACCESS_TARGET,
            width,
            height
        );

        if (!m_scene_render_target) {
            return;
        }

        SDL_SetTextureBlendMode(m_scene_render_target, SDL_BLENDMODE_BLEND);

        m_scene_render_target_width = width;
        m_scene_render_target_height = height;
    }

    void Ui::draw_scene_viewport(Scene& scene, SDL_Renderer* renderer)
    {
        if (!m_show_scene_viewport) {
            scene.set_viewport({});
            return;
        }

        tooling::EditorLayout::scene_viewport();

        if (!ImGui::Begin("Scene", &m_show_scene_viewport, ImGuiWindowFlags_NoScrollbar)) {
            scene.set_viewport({});
            ImGui::End();
            return;
        }

        const ImVec2 viewport_pos = ImGui::GetCursorScreenPos();
        ImVec2 viewport_size = ImGui::GetContentRegionAvail();

        viewport_size.x = std::max(viewport_size.x, 1.0f);
        viewport_size.y = std::max(viewport_size.y, 1.0f);

        SceneViewport viewport{};
        viewport.screen_x = static_cast<int>(viewport_pos.x);
        viewport.screen_y = static_cast<int>(viewport_pos.y);
        viewport.width = static_cast<int>(viewport_size.x);
        viewport.height = static_cast<int>(viewport_size.y);
        viewport.focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

        ensure_scene_render_target(renderer, viewport.width, viewport.height);

        if (m_scene_render_target) {
            ImGui::InvisibleButton("##scene_viewport_input", viewport_size);

            viewport.hovered = ImGui::IsItemHovered();

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddImage(
                reinterpret_cast<ImTextureID>(m_scene_render_target),
                viewport_pos,
                ImVec2(viewport_pos.x + viewport_size.x, viewport_pos.y + viewport_size.y)
            );
        }
        else {
            ImGui::Dummy(viewport_size);
            ImGui::TextUnformatted("Failed to create scene render target.");
            viewport.hovered = false;
        }

        scene.set_viewport(viewport);

        ImGui::End();
    }

    void Ui::render_scene_viewport_content(Scene& scene, SDL_Renderer* renderer)
    {
        const SceneViewport& viewport = scene.get_viewport();

        if (!viewport.has_area()) {
            return;
        }

        ensure_scene_render_target(renderer, viewport.width, viewport.height);

        if (!m_scene_render_target) {
            return;
        }

        SDL_Texture* previous_target = SDL_GetRenderTarget(renderer);

        SDL_SetRenderTarget(renderer, m_scene_render_target);
        SDL_SetRenderDrawColor(renderer, 18, 14, 24, 255);
        SDL_RenderClear(renderer);

        scene.render(renderer);

        SDL_SetRenderTarget(renderer, previous_target);
    }

    void Ui::set_file_status(std::string status, bool is_error)
    {
        m_file_status = std::move(status);
        m_file_status_is_error = is_error;
    }
}
