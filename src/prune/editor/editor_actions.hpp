#pragma once

#include <cstddef>

#include "prune/scene/scene.hpp"

namespace prune {

    struct SelectedObjectActionAvailability {
        bool can_clone_active = false;
        bool can_delete_selection = false;
        std::size_t deletable_count = 0;
    };

    [[nodiscard]] SelectedObjectActionAvailability selected_object_action_availability(Scene& scene);
    bool clone_active_selected_object(Scene& scene, const GridOptions& grid_options);
    bool delete_selected_objects(Scene& scene);

}
