#include "sandbox_scene.hpp"

#include <SDL2/SDL.h>
#include <algorithm>
#include <string>

#include "imgui.h"
#include "prune/core/input.hpp"

namespace prune {

    GameObject* SandboxScene::pick_object_at(int x, int y) noexcept
    {
        auto& objects = m_objects.objects();

        for (auto it = objects.rbegin(); it != objects.rend(); ++it) {
            GameObject& object = *it;

            if (!object.active || !object.visible) {
                continue;
            }

            const RectF bounds = object.bounds();

            const bool inside =
                static_cast<float>(x) >= bounds.x &&
                static_cast<float>(x) < (bounds.x + bounds.width) &&
                static_cast<float>(y) >= bounds.y &&
                static_cast<float>(y) < (bounds.y + bounds.height);

            if (inside) {
                return &object;
            }
        }

        return nullptr;
    }

    void SandboxScene::handle_scene_click(const Input& input)
    {
        if (ImGui::GetIO().WantCaptureMouse) {
            return;
        }

        if (!input.was_mouse_button_pressed(SDL_BUTTON_LEFT)) {
            return;
        }

        GameObject* clicked = pick_object_at(input.mouse_x(), input.mouse_y());
        if (!clicked) {
            return;
        }

        m_objects.select(clicked->id);
    }

    std::string SandboxScene::make_unique_name(std::string desired, GameObjectId ignore_id) const
    {
        if (desired.empty()) {
            desired = "Object";
        }

        auto is_taken = [&](const std::string& name) {
            for (const auto& obj : m_objects.objects()) {
                if (obj.id != ignore_id && obj.name == name) {
                    return true;
                }
            }
            return false;
        };

        if (!is_taken(desired)) {
            return desired;
        }

        int suffix = 1;
        std::string candidate;

        do {
            candidate = desired + " " + std::to_string(suffix++);
        } while (is_taken(candidate));

        return candidate;
    }

    GameObjectId SandboxScene::create_block(float x, float y)
    {
        GameObject block;
        block.name = "Block";
        block.transform.x = x;
        block.transform.y = y;
        block.rectangle.width = 50;
        block.rectangle.height = 50;
        block.rectangle.color[0] = 0.8f;
        block.rectangle.color[1] = 0.5f;
        block.rectangle.color[2] = 0.2f;
        block.active = true;
        block.visible = true;
        block.solid = true;
        block.is_player = false;

        const GameObjectId id = m_objects.create_object(block);

        if (GameObject* created = m_objects.get_by_id(id)) {
            created->name = "Block " + std::to_string(id);
            created->clamp_to_area(m_window_width, m_window_height);
        }

        m_objects.select(id);

        return id;
    }

} // namespace prune