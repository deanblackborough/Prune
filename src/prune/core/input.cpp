#include "input.hpp"

#include <SDL2/SDL.h>

#include <algorithm>

namespace prune {

Input::Input() = default;

void Input::begin_frame() {
    std::fill(m_keys_pressed.begin(), m_keys_pressed.end(), false);
    std::fill(m_keys_released.begin(), m_keys_released.end(), false);

    std::fill(m_mouse_pressed.begin(), m_mouse_pressed.end(), false);
    std::fill(m_mouse_released.begin(), m_mouse_released.end(), false);

    m_mouse_delta_x = 0;
    m_mouse_delta_y = 0;
    m_mouse_wheel_delta = 0.0f;
}

void Input::process_event(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN: {
            const auto scancode = static_cast<std::size_t>(event.key.keysym.scancode);
            if (scancode < kMaxScancodes) {
                if (!event.key.repeat) {
                    if (!m_keys_down[scancode]) {
                        m_keys_pressed[scancode] = true;
                    }
                    m_keys_down[scancode] = true;
                }
            }
            break;
        }

        case SDL_KEYUP: {
            const auto scancode = static_cast<std::size_t>(event.key.keysym.scancode);
            if (scancode < kMaxScancodes) {
                m_keys_down[scancode] = false;
                m_keys_released[scancode] = true;
            }
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            const auto button = static_cast<std::size_t>(event.button.button);
            if (button < kMouseButtonCount) {
                if (!m_mouse_down[button]) {
                    m_mouse_pressed[button] = true;
                }
                m_mouse_down[button] = true;
            }
            break;
        }

        case SDL_MOUSEBUTTONUP: {
            const auto button = static_cast<std::size_t>(event.button.button);
            if (button < kMouseButtonCount) {
                m_mouse_down[button] = false;
                m_mouse_released[button] = true;
            }
            break;
        }

        case SDL_MOUSEMOTION: {
            m_mouse_x = event.motion.x;
            m_mouse_y = event.motion.y;
            m_mouse_delta_x += event.motion.xrel;
            m_mouse_delta_y += event.motion.yrel;
            break;
        }

        case SDL_MOUSEWHEEL: {
            m_mouse_wheel_delta += static_cast<float>(event.wheel.preciseY);
            break;
        }

        default:
            break;
    }
}

bool Input::is_key_down(int scancode) const {
    return scancode >= 0 &&
           static_cast<std::size_t>(scancode) < kMaxScancodes &&
           m_keys_down[static_cast<std::size_t>(scancode)];
}

bool Input::is_key_pressed(int scancode) const {
    return scancode >= 0 &&
           static_cast<std::size_t>(scancode) < kMaxScancodes &&
           m_keys_pressed[static_cast<std::size_t>(scancode)];
}

bool Input::is_key_released(int scancode) const {
    return scancode >= 0 &&
           static_cast<std::size_t>(scancode) < kMaxScancodes &&
           m_keys_released[static_cast<std::size_t>(scancode)];
}

bool Input::is_mouse_button_down(std::uint8_t button) const {
    return button < kMouseButtonCount && m_mouse_down[button];
}

bool Input::is_mouse_button_pressed(std::uint8_t button) const {
    return button < kMouseButtonCount && m_mouse_pressed[button];
}

bool Input::is_mouse_button_released(std::uint8_t button) const {
    return button < kMouseButtonCount && m_mouse_released[button];
}

int Input::mouse_x() const {
    return m_mouse_x;
}

int Input::mouse_y() const {
    return m_mouse_y;
}

int Input::mouse_delta_x() const {
    return m_mouse_delta_x;
}

int Input::mouse_delta_y() const {
    return m_mouse_delta_y;
}

float Input::mouse_wheel_delta() const {
    return m_mouse_wheel_delta;
}

} // namespace prune