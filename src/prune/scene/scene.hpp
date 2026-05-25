#pragma once

#include <span>
#include <string>
#include <string_view>

#include <SDL2/SDL.h>

#include "prune/core/input.hpp"
#include "prune/scene/game_object_manager.hpp"
#include "prune/scene/object_concept.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

    struct WorldSceneContext {
        GridOptions* grid_options = nullptr;
        SceneCamera* camera = nullptr;

        [[nodiscard]] bool available() const noexcept {
            return grid_options != nullptr && camera != nullptr;
        }
    };

    struct ConstWorldSceneContext {
        const GridOptions* grid_options = nullptr;
        const SceneCamera* camera = nullptr;

        [[nodiscard]] bool available() const noexcept {
            return grid_options != nullptr && camera != nullptr;
        }
    };

    struct SceneCreationAction {
        std::string_view id;
        std::string_view label;
    };

    class Scene {
    public:
        virtual ~Scene() = default;

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        virtual void on_enter() = 0;
        virtual void on_exit() = 0;
        virtual void update(float dt, const Input& input) = 0;
        virtual void update_editor(float, const Input&) {}
        virtual void render(SDL_Renderer* renderer) = 0;

        virtual void new_scene() = 0;

        [[nodiscard]] virtual bool save_to_file(std::string_view path, std::string& error) const = 0;
        [[nodiscard]] virtual bool load_from_file(std::string_view path, std::string& error) = 0;
        [[nodiscard]] virtual std::string_view default_file_path() const noexcept = 0;
        [[nodiscard]] virtual std::string_view scene_type_id() const noexcept = 0;

        virtual void set_viewport(const SceneViewport& viewport) noexcept = 0;
        [[nodiscard]] virtual const SceneViewport& get_viewport() const noexcept = 0;
        [[nodiscard]] virtual int get_viewport_width() const noexcept = 0;
        [[nodiscard]] virtual int get_viewport_height() const noexcept = 0;

        [[nodiscard]] virtual std::string_view scene_name() const noexcept = 0;

        [[nodiscard]] virtual bool object_is_selectable(const GameObject& object) const
        {
            return object.editor.selectable && object_concept_for(object).selectable;
        }

        [[nodiscard]] virtual bool object_is_movable(const GameObject& object) const
        {
            const ObjectConcept concept_for = object_concept_for(object);

            return object_is_selectable(object) &&
                object.editor.movable &&
                concept_for.editable &&
                !concept_for.runtime_only &&
                object.identity.type != GameObjectType::Runtime;
        }

        [[nodiscard]] virtual ObjectConcept object_concept_for(const GameObject& object) const
        {
            return object_concepts::fallback_for(object);
        }

        [[nodiscard]] virtual std::string object_role_label(const GameObject& object) const
        {
            return std::string(object_concept_for(object).label);
        }

        [[nodiscard]] virtual std::span<const SceneCreationAction> scene_creation_actions() const noexcept
        {
            return {};
        }

        virtual GameObjectId create_scene_object(std::string_view)
        {
            return k_invalid_game_object_id;
        }

        [[nodiscard]] virtual std::string_view scene_tools_label() const noexcept = 0;
        virtual void draw_scene_tools(bool& open) = 0;
        virtual void draw_viewport_overlays() = 0;

        virtual GameObjectManager& get_object_manager() = 0;

        virtual SceneOptions& get_scene_options() = 0;

        [[nodiscard]] virtual WorldSceneContext world_scene_context() noexcept { return {}; }
        [[nodiscard]] virtual ConstWorldSceneContext world_scene_context() const noexcept { return {}; }

        virtual void draw_scene_inspector(GameObject& selected) = 0;

    protected:
        Scene() = default;
    };
}
