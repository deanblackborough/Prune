#include "prune/scene/game_object.hpp"

#include <algorithm>

namespace prune {

    RectF GameObject::bounds() const noexcept
    {
        return {
            transform.x,
            transform.y,
            static_cast<float>(size.width),
            static_cast<float>(size.height)
        };
    }
} 