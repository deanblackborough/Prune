#pragma once

namespace prune {

  class Input;
  struct Velocity;

  class SimpleShooterPlayerController {
  public:
    [[nodiscard]] Velocity movement_velocity(const Input& input,
                                             float speed) const;
  };
} // namespace prune
