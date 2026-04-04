#include "game_object.hpp"

#include <algorithm>

namespace prune {

    /**
     * @brief Compute the object's axis-aligned bounding rectangle.
     *
     * @return RectF representing the object's bounding rectangle with
     * x = transform.x, y = transform.y, width = rectangle.width (cast to float),
     * and height = rectangle.height (cast to float).
     */
    RectF GameObject::bounds() const noexcept
    {
        return {
            transform.x,
            transform.y,
            static_cast<float>(rectangle.width),
            static_cast<float>(rectangle.height)
        };
    }

    /**
     * @brief Clamp the object's position so its rectangle remains inside a containing area.
     *
     * Adjusts this object's transform.x and transform.y in-place so the object's rectangle
     * lies fully within an area of the specified width and height. If the provided area
     * is smaller than the rectangle in a dimension, the corresponding coordinate is
     * clamped to 0.
     *
     * @param area_width Width of the containing area.
     * @param area_height Height of the containing area.
     */
    void GameObject::clamp_to_area(int area_width, int area_height) noexcept
    {
        const float max_x = std::max(0.0f, static_cast<float>(area_width - rectangle.width));
        const float max_y = std::max(0.0f, static_cast<float>(area_height - rectangle.height));

        transform.x = std::clamp(transform.x, 0.0f, max_x);
        transform.y = std::clamp(transform.y, 0.0f, max_y);
    }

} // namespace prune