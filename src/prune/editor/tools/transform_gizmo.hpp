#pragma once

#include <array>

#include <SDL2/SDL.h>

namespace prune::editor::tools::transform_gizmo {

    constexpr int k_outline_padding = 2;
    constexpr int k_move_handle_size = 12;
    constexpr int k_scale_handle_size = 8;

    enum class ScaleHandle {
        None = 0,
        TopLeft,
        Top,
        TopRight,
        Right,
        BottomRight,
        Bottom,
        BottomLeft,
        Left
    };

    inline constexpr std::array<ScaleHandle, 8> k_scale_handles{
        ScaleHandle::TopLeft,
        ScaleHandle::Top,
        ScaleHandle::TopRight,
        ScaleHandle::Right,
        ScaleHandle::BottomRight,
        ScaleHandle::Bottom,
        ScaleHandle::BottomLeft,
        ScaleHandle::Left
    };

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

    [[nodiscard]] inline SDL_Rect scale_handle_rect(const SDL_Rect& selected_outline, ScaleHandle handle) noexcept
    {
        const int half_size = k_scale_handle_size / 2;
        const int center_x = selected_outline.x + (selected_outline.w / 2);
        const int center_y = selected_outline.y + (selected_outline.h / 2);
        const int right = selected_outline.x + selected_outline.w;
        const int bottom = selected_outline.y + selected_outline.h;

        switch (handle) {
        case ScaleHandle::TopLeft:
            return SDL_Rect{ selected_outline.x - half_size, selected_outline.y - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::Top:
            return SDL_Rect{ center_x - half_size, selected_outline.y - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::TopRight:
            return SDL_Rect{ right - half_size, selected_outline.y - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::Right:
            return SDL_Rect{ right - half_size, center_y - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::BottomRight:
            return SDL_Rect{ right - half_size, bottom - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::Bottom:
            return SDL_Rect{ center_x - half_size, bottom - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::BottomLeft:
            return SDL_Rect{ selected_outline.x - half_size, bottom - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::Left:
            return SDL_Rect{ selected_outline.x - half_size, center_y - half_size, k_scale_handle_size, k_scale_handle_size };
        case ScaleHandle::None:
            break;
        }

        return SDL_Rect{};
    }

    [[nodiscard]] inline bool contains_point(const SDL_Rect& rect, int x, int y) noexcept
    {
        return x >= rect.x &&
            y >= rect.y &&
            x < rect.x + rect.w &&
            y < rect.y + rect.h;
    }

    [[nodiscard]] inline ScaleHandle scale_handle_at_point(const SDL_Rect& selected_outline, int x, int y) noexcept
    {
        for (const ScaleHandle handle : k_scale_handles) {
            if (contains_point(scale_handle_rect(selected_outline, handle), x, y)) {
                return handle;
            }
        }

        return ScaleHandle::None;
    }

    [[nodiscard]] inline bool scale_handle_resizes_left(ScaleHandle handle) noexcept
    {
        return handle == ScaleHandle::TopLeft ||
            handle == ScaleHandle::BottomLeft ||
            handle == ScaleHandle::Left;
    }

    [[nodiscard]] inline bool scale_handle_resizes_right(ScaleHandle handle) noexcept
    {
        return handle == ScaleHandle::TopRight ||
            handle == ScaleHandle::BottomRight ||
            handle == ScaleHandle::Right;
    }

    [[nodiscard]] inline bool scale_handle_resizes_top(ScaleHandle handle) noexcept
    {
        return handle == ScaleHandle::TopLeft ||
            handle == ScaleHandle::TopRight ||
            handle == ScaleHandle::Top;
    }

    [[nodiscard]] inline bool scale_handle_resizes_bottom(ScaleHandle handle) noexcept
    {
        return handle == ScaleHandle::BottomLeft ||
            handle == ScaleHandle::BottomRight ||
            handle == ScaleHandle::Bottom;
    }

}
