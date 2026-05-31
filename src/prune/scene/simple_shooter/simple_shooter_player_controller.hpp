#pragma once

namespace prune {

    class Input;
    struct Velocity;

    class SimpleShooterPlayerController {
    public:
        [[nodiscard]] Velocity movement_velocity(const Input& input) const;

        [[nodiscard]] float speed() const noexcept;
        void set_speed(float speed) noexcept;

    private:
        float m_speed = 96.0f;
    };
}
