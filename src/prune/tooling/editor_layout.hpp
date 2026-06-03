#pragma once

namespace prune::tooling {

    class EditorLayout {
    public:
        static void scene_viewport();
        static void outliner();
        static void scene_panel();
        static void inspector();
        static void stats();
        static void command_history();
        static void options();
        static void controls();
    };
}