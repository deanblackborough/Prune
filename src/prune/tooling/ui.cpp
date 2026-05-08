#include "imgui.h"

#include "prune/scene/sandbox_scene.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    Ui::~Ui()
    {
        destroy_scene_render_target();
    }

    void Ui::build(SandboxScene& scene, SDL_Renderer* renderer) 
    {
        draw_scene_viewport(scene, renderer);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New Scene")) {
                    scene.new_scene();
                    m_file_status = "Created new scene";
                    m_file_status_is_error = false;
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene")) {
                    std::string error;
                    if (scene.save_to_file(kSceneFilePath, error)) {
                        m_file_status = "Saved scene to sandbox_scene.yml";
                        m_file_status_is_error = false;
                    }
                    else {
                        m_file_status = "Save failed: " + error;
                        m_file_status_is_error = true;
                    }
                }

                if (ImGui::MenuItem("Load Scene")) {
                    std::string error;
                    if (scene.load_from_file(kSceneFilePath, error)) {
                        m_file_status = "Loaded scene from sandbox_scene.yml";
                        m_file_status_is_error = false;
                    }
                    else {
                        m_file_status = "Load failed: " + error;
                        m_file_status_is_error = true;
                    }
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
                ImGui::MenuItem("Simple Shooter", nullptr, &m_show_simple_shooter);
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
            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(
                viewport->GetCenter(),
                ImGuiCond_FirstUseEver,
                ImVec2(0.5f, 0.5f)
            );
            ImGui::SetNextWindowSize(ImVec2(306.0, 308.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Options", &m_show_view_grid_options)) {
                m_options.draw(scene.get_scene_options(), scene.get_grid_options(), scene.get_camera());
            }
            ImGui::End();
        }

        if (m_show_outliner) {
            ImGui::SetNextWindowPos(ImVec2(945.0f, 25.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(330.0f, 175.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Outliner", &m_show_outliner)) {
                const Camera& camera = scene.get_camera().active();
                GridOptions& grid_options = scene.get_grid_options();

                m_outliner.draw(
                    scene.get_object_manager(),
                    camera.x,
                    camera.y,
                    scene.get_viewport_width(),
                    scene.get_viewport_height(),
                    grid_options.snap_to_grid,
                    grid_options.grid_size
                );
            }
            ImGui::End();
        }

        if (m_show_inspector) {
            ImGui::SetNextWindowPos(ImVec2(945.0f, 390.f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(330.0f, 330.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Inspector", &m_show_inspector)) {
                m_inspector.draw(
                    scene.get_object_manager(),
                    scene.get_player_id(),
                    scene.get_player_controller(),
                    scene.get_grid_options(),
                    scene.get_camera().active()
                );
            }
            ImGui::End();
        }

        if (m_show_simple_shooter) {
            draw_simple_shooter_panel(scene);
        }

        if (m_show_controls) {

            ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(
                viewport->GetCenter(),
                ImGuiCond_FirstUseEver,
                ImVec2(0.5f, 0.5f)
            );
            ImGui::SetNextWindowSize(ImVec2(377.0f, 165.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Controls", &m_show_controls)) {
                m_controls.draw();
            }
            ImGui::End();
        }

        if (m_show_stats) {
            ImGui::SetNextWindowPos(ImVec2(10.0f, 34.0f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(300.0f, 320.0f), ImGuiCond_FirstUseEver);

            if (ImGui::Begin("Stats", &m_show_stats)) {
                m_stats.draw(
                    scene.get_object_manager(),
                    scene.get_player_id(),
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

    void Ui::draw_scene_viewport(SandboxScene& scene, SDL_Renderer* renderer)
    {
        if (!m_show_scene_viewport) {
            scene.set_viewport({});
            return;
        }

        ImGui::SetNextWindowPos(ImVec2(5.0f, 25.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(930.0f, 700.0f), ImGuiCond_FirstUseEver);

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

    void Ui::render_scene_viewport_content(SandboxScene& scene, SDL_Renderer* renderer)
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

    void Ui::draw_simple_shooter_panel(SandboxScene& scene)
    {
        ImGui::SetNextWindowPos(ImVec2(945.0f, 205.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(330.0f, 180.0f), ImGuiCond_FirstUseEver);

        if (!ImGui::Begin("Simple Shooter", &m_show_simple_shooter)) {
            ImGui::End();
            return;
        }

        SimpleShooterOptions& options = scene.get_simple_shooter_options();

        ImGui::TextUnformatted("Vertical slice controls");
        ImGui::Separator();

        ImGui::SliderFloat("Enemy speed", &options.enemy_speed, 0.0f, 160.0f, "%.1f");
        ImGui::SliderFloat("Bullet speed", &options.bullet_speed, 40.0f, 400.0f, "%.1f");
        ImGui::SliderFloat("Bullet lifetime", &options.bullet_lifetime, 0.2f, 4.0f, "%.2f");

        const GameObject* enemy = scene.enemy_object();
        ImGui::Text("Enemy: %s", (enemy && enemy->active) ? "alive" : "dead");
        ImGui::Text("Bullets: %d", scene.bullet_count());

        if (ImGui::Button("Reset enemy")) {
            scene.reset_simple_shooter();
        }

        ImGui::End();
    }
} 