#pragma once

#include <string>

namespace prune::tooling {

    bool draw_sprite_picker(
        const char* label,
        const char* id,
        std::string& selected_sprite_key
    );

}
