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

    void GameObject::clamp_to_area(int area_width, int area_height) noexcept
    {
        const float max_x = std::max(0.0f, static_cast<float>(area_width - rectangle.width));
        const float max_y = std::max(0.0f, static_cast<float>(area_height - rectangle.height));

        transform.x = std::clamp(transform.x, 0.0f, max_x);
        transform.y = std::clamp(transform.y, 0.0f, max_y);
    }

} // namespace prune