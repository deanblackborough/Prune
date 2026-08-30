#pragma once

#include "prune/scene/artillery/artillery_state.hpp"

namespace prune {

  class Artillery {
  public:
    [[nodiscard]] bool draw(ArtilleryState& state);
  };

} // namespace prune
