#pragma once

namespace prune::tooling {

    enum class EditorTheme {
        PrunePurple = 0
    };

    void apply_editor_theme(EditorTheme theme = EditorTheme::PrunePurple);

}