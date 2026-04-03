#include "input.hpp"

namespace prune {

void Input::begin_frame()
{
    m_keys_pressed.fill(false);
    m_keys_released.fill(false);

    m_mouse_pressed.fill(false);
    m_mouse_released.fill(false);

    m_mouse_delta_x = 0;
    m_mouse_delta_y = 0;
    m_mouse_wheel_delta = 0.0f;
}

void Input::process_event(const SDL_Event& event)
{
    switch (event.type) {
        case SDL_KEYDOWN: {
            if (!event.key.repeat) {
                const auto scancode = event.key.keysym.scancode;
                m_keys_down[scancode] = true;
                m_keys_pressed[scancode] = true;
            }
            break;
        }

        case SDL_KEYUP: {
            const auto scancode = event.key.keysym.scancode;
            m_keys_down[scancode] = false;
            m_keys_released[scancode] = true;
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            const auto button = static_cast<std::size_t>(event.button.button);
            if (button < kMouseButtonCount) {
                m_mouse_down[button] = true;
                m_mouse_pressed[button] = true;
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

bool Input::is_key_down(SDL_Scancode key) const
{
    return m_keys_down[key];
}

bool Input::was_key_pressed(SDL_Scancode key) const
{
    return m_keys_pressed[key];
}

bool Input::was_key_released(SDL_Scancode key) const
{
    return m_keys_released[key];
}

bool Input::is_mouse_button_down(std::uint8_t button) const
{
    return button < kMouseButtonCount && m_mouse_down[button];
}

bool Input::was_mouse_button_pressed(std::uint8_t button) const
{
    return button < kMouseButtonCount && m_mouse_pressed[button];
}

bool Input::was_mouse_button_released(std::uint8_t button) const
{
    return button < kMouseButtonCount && m_mouse_released[button];
}

int Input::mouse_x() const
{
    return m_mouse_x;
}

int Input::mouse_y() const
{
    return m_mouse_y;
}

int Input::mouse_delta_x() const
{
    return m_mouse_delta_x;
}

int Input::mouse_delta_y() const
{
    return m_mouse_delta_y;
}

float Input::mouse_wheel_delta() const
{
    return m_mouse_wheel_delta;
}

} // namespace prune