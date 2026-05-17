#include <cmath>

#include "prune/scene/collision.hpp"

namespace prune::collision {

    bool is_overlapping(const GameObject& a, const GameObject& b) noexcept
    {
        const RectF a_bounds = a.bounds();
        const RectF b_bounds = b.bounds();

        return a_bounds.x < (b_bounds.x + b_bounds.width) &&
            (a_bounds.x + a_bounds.width) > b_bounds.x &&
            a_bounds.y < (b_bounds.y + b_bounds.height) &&
            (a_bounds.y + a_bounds.height) > b_bounds.y;
    }

    void resolve_against_solids(GameObject& object, const GameObjectManager& objects)
    {
        for (const auto& other : objects.objects()) {
            if (
                other.identity.id == object.identity.id ||
                !other.lifecycle.active ||
                !other.collision.solid
                ) {
                continue;
            }

            if (!is_overlapping(object, other)) {
                continue;
            }

            const RectF object_bounds = object.bounds();
            const RectF other_bounds = other.bounds();

            const float overlap_left = (object_bounds.x + object_bounds.width) - other_bounds.x;
            const float overlap_right = (other_bounds.x + other_bounds.width) - object_bounds.x;
            const float overlap_top = (object_bounds.y + object_bounds.height) - other_bounds.y;
            const float overlap_bottom = (other_bounds.y + other_bounds.height) - object_bounds.y;

            const float resolve_x = (overlap_left < overlap_right) ? -overlap_left : overlap_right;
            const float resolve_y = (overlap_top < overlap_bottom) ? -overlap_top : overlap_bottom;

            if (std::abs(resolve_x) < std::abs(resolve_y)) {
                object.transform.x += resolve_x;
                object.motion.velocity.x = 0.0f;
            }
            else {
                object.transform.y += resolve_y;
                object.motion.velocity.y = 0.0f;
            }
        }
    }
}
