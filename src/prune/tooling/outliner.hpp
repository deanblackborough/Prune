#pragma once

#include <array>

#include "prune/scene/game_object.hpp"
#include "prune/scene/game_object_manager.hpp"

namespace prune {

    class Outliner {
    public:
        void draw(GameObjectManager& objects);
    private:
        static std::string object_label(const GameObject& object);
        bool contains_case_insensitive(std::string_view text, std::string_view query) const;

        std::array<char, 128> m_object_search{};
    };
}