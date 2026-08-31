#pragma once

#include "prune/core/defaults.hpp"

namespace prune {

  struct GridOptions {
    bool show_grid = true;
    bool snap_to_grid = true;
    int grid_size = k_default_object_size;
    int nudge_step = 8;
    int shift_nudge_steps = 4;

    int min_grid_size = k_min_object_size;
    int max_grid_size = k_max_object_size;
    int min_nudge_step = 4;
    int max_nudge_step = 64;
  };

} // namespace prune
