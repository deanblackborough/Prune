#include "game_object.hpp"

#include <algorithm>

namespace prune {

    RectF GameObject::bounds() const noexcept
    {
        return {
            transform.x,
            transform.y,
            static_cast<float>(rectangle.width),
            static_cast<float>(rectangle.height)
        };
    }

} // namespace prune