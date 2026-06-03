#pragma once

#include <SDL2/SDL.h>

namespace prune::editor::tools::transform_gizmo {

    constexpr int k_outline_padding = 2;
    constexpr int k_move_handle_size = 12;

    [[nodiscard]] inline SDL_Rect selected_outline_rect(const SDL_Rect& object_rect) noexcept
    {
        return SDL_Rect{
            object_rect.x - k_outline_padding,
            object_rect.y - k_outline_padding,
            object_rect.w + (k_outline_padding * 2),
            object_rect.h + (k_outline_padding * 2)
        };
    }

    [[nodiscard]] inline SDL_Rect move_handle_rect(const SDL_Rect& selected_outline) noexcept
    {
        return SDL_Rect{
            selected_outline.x - k_move_handle_size,
            selected_outline.y + selected_outline.h - k_move_handle_size,
            k_move_handle_size,
            k_move_handle_size
        };
    }

    [[nodiscard]] inline bool contains_point(const SDL_Rect& rect, int x, int y) noexcept
    {
        return x >= rect.x &&
            y >= rect.y &&
            x < rect.x + rect.w &&
            y < rect.y + rect.h;
    }

}
