#pragma once

namespace prune {

    class Input;
    struct GameObject;

    class PlayerController {
    public:
        void update(
            GameObject& object,
            float dt,
            const Input& input,
            int world_width,
            int world_height
        ) const;

        [[nodiscard]] float speed() const noexcept;
        void set_speed(float speed) noexcept;

    private:
        float m_speed = 240.0f;
    };

} // namespace prune