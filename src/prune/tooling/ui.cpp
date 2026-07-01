#include <algorithm>
#include <cstddef>
#include <span>
#include <string>
#include <string_view>
#include <utility>

#include "imgui.h"

#include "prune/editor/editor_actions.hpp"
#include "prune/editor/editor_tool.hpp"
#include "prune/scene/scene.hpp"
#include "prune/tooling/editor_layout.hpp"
#include "prune/tooling/ui.hpp"

namespace prune {

    namespace {

        constexpr float k_tool_palette_margin = 10.0f;
        constexpr float k_tool_palette_padding = 8.0f;
        constexpr float k_tool_button_min_width = 74.0f;
        constexpr std::size_t k_scene_creation_action_columns = 2;

        struct ToolPaletteLayout {
            ImVec2 size{};
            float selection_action_button_width = 0.0f;
            std::size_t selection_action_columns = 0;
            bool has_selection_actions = false;
            bool show_clone_action = false;
            bool show_delete_action = false;
            float scene_creation_button_width = 0.0f;
            std::size_t scene_creation_action_columns = 0;
            bool has_scene_creation_actions = false;
        };

        [[nodiscard]] float tool_button_width(std::string_view label)
        {
            const ImGuiStyle& style = ImGui::GetStyle();
            const ImVec2 label_size = ImGui::CalcTextSize(label.data(), label.data() + label.size());

            return std::max(
                k_tool_button_min_width,
                label_size.x + (style.FramePadding.x * 2.0f)
            );
        }

        [[nodiscard]] ToolPaletteLayout editor_tool_palette_layout(Scene& scene, const GridOptions* grid_options)
        {
            const ImGuiStyle& style = ImGui::GetStyle();
            const std::span<const SceneCreationAction> actions = scene.scene_creation_actions();

            const float editor_tools_width =
                (k_tool_button_min_width * 3.0f) + (style.ItemSpacing.x * 2.0f);

            ToolPaletteLayout layout{};
            float content_width = editor_tools_width;
            float content_height =
                ImGui::GetTextLineHeight() +
                style.ItemSpacing.y +
                ImGui::GetFrameHeight();

            const SelectedObjectActionAvailability selection_actions = selected_object_action_availability(scene);
            layout.show_clone_action = selection_actions.can_clone_active && grid_options != nullptr;
            layout.show_delete_action = selection_actions.can_delete_selection;
            layout.has_selection_actions = layout.show_clone_action || layout.show_delete_action;

            if (layout.has_selection_actions) {
                layout.selection_action_columns =
                    (layout.show_clone_action && layout.show_delete_action) ? std::size_t{ 2 } : std::size_t{ 1 };
                layout.selection_action_button_width = std::max(
                    tool_button_width("Clone"),
                    tool_button_width("Delete")
                );

                const float selection_actions_width =
                    (layout.selection_action_button_width * static_cast<float>(layout.selection_action_columns)) +
                    (style.ItemSpacing.x * static_cast<float>(layout.selection_action_columns - 1));

                content_width = std::max(content_width, selection_actions_width);
                content_height +=
                    (style.ItemSpacing.y * 2.0f) +
                    1.0f +
                    ImGui::GetTextLineHeight() +
                    style.ItemSpacing.y +
                    ImGui::GetFrameHeight();
            }

            if (!actions.empty()) {
                layout.has_scene_creation_actions = true;
                layout.scene_creation_action_columns = std::min(
                    k_scene_creation_action_columns,
                    actions.size()
                );
                layout.scene_creation_button_width = k_tool_button_min_width;

                for (const SceneCreationAction& action : actions) {
                    layout.scene_creation_button_width = std::max(
                        layout.scene_creation_button_width,
                        tool_button_width(action.label)
                    );
                }

                const std::size_t rows =
                    (actions.size() + layout.scene_creation_action_columns - 1) /
                    layout.scene_creation_action_columns;

                const float scene_creation_width =
                    (layout.scene_creation_button_width * static_cast<float>(layout.scene_creation_action_columns)) +
                    (style.ItemSpacing.x * static_cast<float>(layout.scene_creation_action_columns - 1));

                content_width = std::max(content_width, scene_creation_width);
                content_height +=
                    (style.ItemSpacing.y * 2.0f) +
                    1.0f +
                    ImGui::GetTextLineHeight() +
                    style.ItemSpacing.y +
                    (ImGui::GetFrameHeight() * static_cast<float>(rows)) +
                    (style.ItemSpacing.y * static_cast<float>(rows - 1));
            }

            layout.size = ImVec2(
                (k_tool_palette_padding * 2.0f) + content_width,
                (k_tool_palette_padding * 2.0f) + content_height
            );

            return layout;
        }

        [[nodiscard]] bool point_in_rect(const ImVec2& point, const ImVec2& min, const ImVec2& max) noexcept
        {
            return point.x >= min.x && point.y >= min.y && point.x < max.x && point.y < max.y;
        }

        void draw_editor_tool_button(Scene& scene, EditorTool tool)
        {
            const bool active = scene.current_editor_tool() == tool;

            if (active) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
            }

            if (ImGui::Button(editor_tool_label(tool), ImVec2(k_tool_button_min_width, 0.0f))) {
                scene.set_current_editor_tool(tool);
            }

            if (active) {
                ImGui::PopStyleColor(2);
            }
        }

        void draw_scene_creation_action_button(
            Scene& scene,
            const SceneCreationAction& action,
            float button_width
        )
        {
            const std::string button_text(action.label);
            ImGui::PushID(action.id.data(), action.id.data() + action.id.size());

            if (ImGui::Button(button_text.c_str(), ImVec2(button_width, 0.0f))) {
                scene.execute_scene_creation_action(action.id);
            }

            ImGui::PopID();
        }

        void draw_editor_tool_palette(
            Scene& scene,
            const ToolPaletteLayout& layout,
            const GridOptions* grid_options,
            const ImVec2& palette_min,
            const ImVec2& palette_max
        )
        {
            const ImGuiStyle& style = ImGui::GetStyle();
            ImDrawList* draw_list = ImGui::GetWindowDrawList();

            draw_list->AddRectFilled(
                palette_min,
                palette_max,
                ImGui::GetColorU32(ImGuiCol_WindowBg, 0.94f),
                style.WindowRounding
            );
            draw_list->AddRect(
                palette_min,
                palette_max,
                ImGui::GetColorU32(ImGuiCol_Border),
                style.WindowRounding
            );

            ImGui::SetCursorScreenPos(ImVec2(
                palette_min.x + k_tool_palette_padding,
                palette_min.y + k_tool_palette_padding
            ));

            ImGui::BeginGroup();
            ImGui::TextUnformatted("Mode");
            draw_editor_tool_button(scene, EditorTool::Select);
            ImGui::SameLine();
            draw_editor_tool_button(scene, EditorTool::Move);
            ImGui::SameLine();
            draw_editor_tool_button(scene, EditorTool::Scale);

            if (layout.has_selection_actions) {
                ImGui::Separator();
                ImGui::TextUnformatted("Selected");

                bool rendered_button = false;

                if (layout.show_clone_action && grid_options != nullptr) {
                    if (ImGui::Button("Clone", ImVec2(layout.selection_action_button_width, 0.0f))) {
                        clone_active_selected_object(scene, *grid_options);
                    }
                    rendered_button = true;
                }

                if (layout.show_delete_action) {
                    if (rendered_button) {
                        ImGui::SameLine();
                    }

                    if (ImGui::Button("Delete", ImVec2(layout.selection_action_button_width, 0.0f))) {
                        delete_selected_objects(scene);
                    }
                }
            }

            if (layout.has_scene_creation_actions) {
                ImGui::Separator();
                ImGui::TextUnformatted("Create");

                const std::span<const SceneCreationAction> actions = scene.scene_creation_actions();
                for (std::size_t index = 0; index < actions.size(); ++index) {
                    if (index > 0 && index % layout.scene_creation_action_columns != 0) {
                        ImGui::SameLine();
                    }

                    draw_scene_creation_action_button(
                        scene,
                        actions[index],
                        layout.scene_creation_button_width
                    );
                }
            }

            ImGui::EndGroup();
        }

    }

    Ui::~Ui()
    {
        destroy_scene_render_target();
    }

    void Ui::build(
        Scene& scene,
        SDL_Renderer* renderer,
        bool& new_scene_requested,
        SceneType& new_scene_type,
        bool& load_scene_requested,
        bool& audio_enabled,
        bool audio_available
    )
    {
        draw_scene_viewport(scene, renderer);

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::BeginMenu("New Scene")) {
                    for (const SceneDescriptor& descriptor : k_scene_descriptors) {
                        if (ImGui::MenuItem(descriptor.label.data())) {
                            new_scene_type = descriptor.type;
                            new_scene_requested = true;
                        }
                    }

                    ImGui::EndMenu();
                }

                ImGui::Separator();

                if (ImGui::MenuItem("Save Scene")) {
                    std::string error;
                    const std::string scene_file_path{ scene.default_file_path() };

                    if (scene.save_to_file(scene_file_path, error)) {
                        m_file_status = "Saved scene to " + scene_file_path;
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
                    ImGui::TextWrapped("Status: %s", m_file_status.c_str());
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit")) {
                const EditorCommandHistory& history = scene.editor_command_history();

                if (ImGui::MenuItem("Undo", "Ctrl+Z", false, history.can_undo())) {
                    scene.undo_editor_command();
                }

                if (ImGui::MenuItem("Redo", "Ctrl+Y", false, history.can_redo())) {
                    scene.redo_editor_command();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Audio")) {
                if (!audio_available) {
                    ImGui::BeginDisabled();
                }

                ImGui::MenuItem("Sound Enabled", nullptr, &audio_enabled);

                if (!audio_available) {
                    ImGui::EndDisabled();
                    ImGui::TextDisabled("Audio device unavailable");
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::MenuItem("Scene", nullptr, &m_show_scene_viewport);
                ImGui::MenuItem("Outliner", nullptr, &m_show_outliner);
                ImGui::MenuItem("Inspector", nullptr, &m_show_inspector);
                ImGui::MenuItem(scene.scene_tools_label().data(), nullptr, &m_show_scene_tools);
                ImGui::MenuItem("Command History", nullptr, &m_show_command_history);
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

        const ImGuiIO& io = ImGui::GetIO();
        if (!io.WantTextInput && !io.WantCaptureKeyboard && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z, false)) {
            scene.undo_editor_command();
        }
        if (!io.WantTextInput && !io.WantCaptureKeyboard && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y, false)) {
            scene.redo_editor_command();
        }

        if (m_show_view_grid_options) {
            tooling::EditorLayout::options();

            if (ImGui::Begin("Options", &m_show_view_grid_options)) {
                WorldSceneContext context = scene.world_scene_context();
                if (context.available()) {
                    m_options.draw(scene.get_scene_options(), *context.grid_options, *context.camera);
                }
                else {
                    ImGui::TextUnformatted("This scene does not expose generic world options.");
                }
            }
            ImGui::End();
        }

        if (m_show_outliner) {
            tooling::EditorLayout::outliner();

            if (ImGui::Begin("Outliner", &m_show_outliner)) {
                m_outliner.draw(scene.get_object_manager(), scene);
            }
            ImGui::End();
        }

        if (m_show_inspector) {
            tooling::EditorLayout::inspector();

            if (ImGui::Begin("Inspector", &m_show_inspector)) {
                WorldSceneContext context = scene.world_scene_context();
                if (context.available()) {
                    m_inspector.draw(
                        scene,
                        context.camera->active()
                    );
                }

                if (GameObject* selected = scene.get_object_manager().selected_object()) {
                    scene.draw_scene_inspector(*selected);
                }
            }

            ImGui::End();
        }

        if (m_show_scene_tools) {
            scene.draw_scene_tools(m_show_scene_tools);
        }

        if (m_show_command_history) {
            tooling::EditorLayout::command_history();

            if (ImGui::Begin("Command History", &m_show_command_history)) {
                m_command_history.draw(scene);
            }
            ImGui::End();
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
                WorldSceneContext context = scene.world_scene_context();
                if (context.available()) {
                    m_stats.draw(
                        scene.get_object_manager(),
                        scene.get_viewport(),
                        *context.camera
                    );
                }
                else {
                    ImGui::TextUnformatted("This scene does not expose generic world stats.");
                }
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

        const ImGuiIO& io = ImGui::GetIO();
        viewport.keyboard_input_enabled =
            viewport.focused &&
            !io.WantCaptureKeyboard &&
            !io.WantTextInput;

        ensure_scene_render_target(renderer, viewport.width, viewport.height);

        if (m_scene_render_target) {
            const ImVec2 viewport_max{
                viewport_pos.x + viewport_size.x,
                viewport_pos.y + viewport_size.y
            };
            const WorldSceneContext context = scene.world_scene_context();
            const GridOptions* grid_options = context.available() ? context.grid_options : nullptr;
            const ToolPaletteLayout tool_palette_layout = editor_tool_palette_layout(scene, grid_options);
            const ImVec2 palette_min{
                viewport_max.x - k_tool_palette_margin - tool_palette_layout.size.x,
                viewport_pos.y + k_tool_palette_margin
            };
            const ImVec2 palette_max{
                palette_min.x + tool_palette_layout.size.x,
                palette_min.y + tool_palette_layout.size.y
            };
            const bool mouse_over_tool_palette = point_in_rect(io.MousePos, palette_min, palette_max);

            bool viewport_input_hovered = false;

            if (mouse_over_tool_palette) {
                ImGui::Dummy(viewport_size);
            }
            else {
                ImGui::InvisibleButton("##scene_viewport_input", viewport_size);
                viewport_input_hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
            }

            viewport.hovered =
                viewport_input_hovered &&
                ImGui::IsMouseHoveringRect(viewport_pos, viewport_max) &&
                !mouse_over_tool_palette;

            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            draw_list->AddImage(
                reinterpret_cast<ImTextureID>(m_scene_render_target),
                viewport_pos,
                viewport_max
            );

            scene.set_viewport(viewport);
            scene.draw_viewport_overlays();
            draw_editor_tool_palette(scene, tool_palette_layout, grid_options, palette_min, palette_max);
        }
        else {
            ImGui::Dummy(viewport_size);
            ImGui::TextUnformatted("Failed to create scene render target.");
            viewport.hovered = false;
        }

        if (!m_scene_render_target) {
            scene.set_viewport(viewport);
        }

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
