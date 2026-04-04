#include "sandbox_scene.hpp"

#include "imgui.h"
#include "input.hpp"

#include <SDL2/SDL.h>
#include <string>

namespace prune {

    /**
     * @brief Construct a SandboxScene with the given window dimensions.
     *
     * Stores the window width and height for use when constraining object positions,
     * rendering bounds, and inspector UI ranges.
     *
     * @param window_width Width of the application window in pixels.
     * @param window_height Height of the application window in pixels.
     */
    SandboxScene::SandboxScene(int window_width, int window_height)
        : m_window_width(window_width)
        , m_window_height(window_height)
    {
    }

    /**
     * @brief Prepare the sandbox scene with its initial objects.
     *
     * Clears any existing scene objects, creates a default "Player" GameObject
     * (positioned at 100,100 with a 50x50 rectangle and color RGB(0.3,0.8,0.5)),
     * and creates a "Static Block" GameObject (positioned at 420,220 with a 50x50
     * rectangle and color RGB(0.8,0.5,0.2)). The index of the created player is
     * stored in m_player_index.
     */
    void SandboxScene::on_enter()
    {
        m_objects.clear();

        GameObject player;
        player.name = "Player";
        player.transform.x = 100.0f;
        player.transform.y = 100.0f;
        player.rectangle.width = 50;
        player.rectangle.height = 50;
        player.rectangle.color[0] = 0.3f;
        player.rectangle.color[1] = 0.8f;
        player.rectangle.color[2] = 0.5f;

        m_player_index = m_objects.create_object(player);

        GameObject block;
        block.name = "Static Block";
        block.transform.x = 420.0f;
        block.transform.y = 220.0f;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;

        m_objects.create_object(block);
    }

    /**
     * @brief Advance the player object's simulation for the current frame.
     *
     * If a player object exists in the scene, update its movement and state based
     * on the elapsed time and the current input, constrained to the scene window.
     * If no player object is present, the call has no effect.
     *
     * @param dt Time elapsed since the previous frame, in seconds.
     * @param input Current input state (keyboard, mouse, controller, etc.) used to drive player behavior.
     */
    void SandboxScene::update(float dt, const Input& input)
    {
        if (GameObject* player = player_object()) {
            m_player_controller.update(
                *player,
                dt,
                input,
                m_window_width,
                m_window_height
            );
        }
    }

    /**
     * @brief Renders all scene objects as filled rectangles using their stored transform, size, and color.
     *
     * Each object's transform.x/y set the rectangle's top-left position, rectangle.width/height set its size,
     * and rectangle.color (components in the range [0, 1]) are scaled to 0–255 RGB with full alpha when drawing.
     *
     * @param renderer SDL renderer used to draw the objects.
     */
    void SandboxScene::render(SDL_Renderer* renderer)
    {
        for (const auto& object : m_objects.objects()) {
            SDL_Rect rect{
                static_cast<int>(object.transform.x),
                static_cast<int>(object.transform.y),
                object.rectangle.width,
                object.rectangle.height
            };

            SDL_SetRenderDrawColor(
                renderer,
                static_cast<Uint8>(object.rectangle.color[0] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[1] * 255.0f),
                static_cast<Uint8>(object.rectangle.color[2] * 255.0f),
                255
            );

            SDL_RenderFillRect(renderer, &rect);
        }
    }

    /**
     * @brief Render and handle the scene inspector UI for selecting and editing scene objects.
     *
     * Displays a list of all scene objects, allows the user to select one, and exposes controls to
     * edit the selected object's position, rectangle size, and color. When the selected object is
     * the player, also exposes a control to view and adjust the player's movement speed. After
     * user edits, the selected object is clamped to the scene window bounds.
     */
    void SandboxScene::draw_inspector_ui()
    {
        ImGui::TextUnformatted("Objects");
        ImGui::Separator();

        for (std::size_t index = 0; index < m_objects.count(); ++index) {
            const GameObject* object = m_objects.get(index);
            if (!object) {
                continue;
            }

            const bool is_selected = index == m_objects.selected_index();
            if (ImGui::Selectable(object->name.c_str(), is_selected)) {
                m_objects.select(index);
            }
        }

        ImGui::Spacing();

        GameObject* selected = m_objects.selected_object();
        if (!selected) {
            ImGui::TextUnformatted("No object selected.");
            return;
        }

        ImGui::Separator();
        ImGui::Text("Selected: %s", selected->name.c_str());

        ImGui::Spacing();
        ImGui::TextUnformatted("Position");

        const auto max_x = static_cast<float>(m_window_width - selected->rectangle.width);
        const auto max_y = static_cast<float>(m_window_height - selected->rectangle.height);

        ImGui::SliderFloat("X", &selected->transform.x, 0.0f, std::max(0.0f, max_x));
        ImGui::SliderFloat("Y", &selected->transform.y, 0.0f, std::max(0.0f, max_y));

        ImGui::Separator();

        ImGui::TextUnformatted("Rectangle");
        ImGui::SliderInt("Width", &selected->rectangle.width, 10, 200);
        ImGui::SliderInt("Height", &selected->rectangle.height, 10, 200);
        ImGui::ColorEdit3("Colour", selected->rectangle.color);

        if (m_objects.selected_index() == m_player_index) {
            float speed = m_player_controller.speed();
            if (ImGui::SliderFloat("Move Speed", &speed, 50.0f, 600.0f, "%.1f")) {
                m_player_controller.set_speed(speed);
            }
        }

        selected->clamp_to_area(m_window_width, m_window_height);
    }

    /**
     * @brief Displays runtime debug information in the ImGui interface.
     *
     * Shows the current window dimensions and the number of scene objects.
     * If a player object exists, also displays a "Player" section with the player's position and the configured move speed.
     */
    void SandboxScene::draw_debug_ui()
    {
        ImGui::TextUnformatted("Sandbox");
        ImGui::Text("Window: %d x %d", m_window_width, m_window_height);
        ImGui::Text("Object count: %d", static_cast<int>(m_objects.count()));

        if (const GameObject* player = player_object()) {
            ImGui::Separator();
            ImGui::TextUnformatted("Player");
            ImGui::Text("Position: %.1f, %.1f", player->transform.x, player->transform.y);
            ImGui::Text("Speed: %.1f", m_player_controller.speed());
        }
    }

    /**
     * @brief Get the scene's current player GameObject pointer.
     *
     * @return GameObject* Pointer to the player GameObject, or `nullptr` if the player index is invalid or the object does not exist.
     */
    GameObject* SandboxScene::player_object() noexcept
    {
        return m_objects.get(m_player_index);
    }

    /**
     * @brief Retrieve the scene's player GameObject by its stored index.
     *
     * @return const GameObject* Pointer to the player GameObject, or `nullptr` if the stored index is invalid or no player exists.
     */
    const GameObject* SandboxScene::player_object() const noexcept
    {
        return m_objects.get(m_player_index);
    }

} // namespace prune