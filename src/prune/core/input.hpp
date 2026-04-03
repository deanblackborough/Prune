#pragma once

#include <SDL2/SDL.h>

#include <array>
#include <cstdint>

namespace prune {

    class Input {
    public:
        void begin_frame();
        void process_event(const SDL_Event& event);

        [[nodiscard]] bool is_key_down(SDL_Scancode key) const;
        [[nodiscard]] bool was_key_pressed(SDL_Scancode key) const;
        [[nodiscard]] bool was_key_released(SDL_Scancode key) const;

        [[nodiscard]] bool is_mouse_button_down(std::uint8_t button) const;
        [[nodiscard]] bool was_mouse_button_pressed(std::uint8_t button) const;
        [[nodiscard]] bool was_mouse_button_released(std::uint8_t button) const;

        [[nodiscard]] int mouse_x() const;
        [[nodiscard]] int mouse_y() const;
        [[nodiscard]] int mouse_delta_x() const;
        [[nodiscard]] int mouse_delta_y() const;
        [[nodiscard]] float mouse_wheel_delta() const;

    private:
        static constexpr std::size_t kMouseButtonCount = 8;

        std::array<bool, SDL_NUM_SCANCODES> m_keys_down{};
        std::array<bool, SDL_NUM_SCANCODES> m_keys_pressed{};
        std::array<bool, SDL_NUM_SCANCODES> m_keys_released{};

        std::array<bool, kMouseButtonCount> m_mouse_down{};
        std::array<bool, kMouseButtonCount> m_mouse_pressed{};
        std::array<bool, kMouseButtonCount> m_mouse_released{};

        int m_mouse_x{0};
        int m_mouse_y{0};
        int m_mouse_delta_x{0};
        int m_mouse_delta_y{0};
        float m_mouse_wheel_delta{0.0f};
    };

} // namespace prune