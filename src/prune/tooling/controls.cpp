#include "prune/tooling/controls.hpp"
#include "prune/tooling/imgui/layout.hpp"

namespace prune {
    void Controls::draw()
    {
        tooling::imgui::layout::header_text("Controls");
        tooling::imgui::layout::text_wrapped("Simple Shooter");
        tooling::imgui::layout::text_wrapped("WASD keys move the player");
        tooling::imgui::layout::text_wrapped("Space fires one projectile in the current facing direction");
        tooling::imgui::layout::text_wrapped("Walls block player movement and stop projectiles");
        tooling::imgui::layout::text_wrapped("Projectiles reset the enemy when they hit");
        tooling::imgui::layout::text_wrapped("Enemy/player contact does not damage or reset the player in this slice");

        tooling::imgui::layout::spacing(2);

        tooling::imgui::layout::text_wrapped("Platformer");
        tooling::imgui::layout::text_wrapped("A/D keys move the player left and right");
        tooling::imgui::layout::text_wrapped("W or Space jumps, but only while grounded");
        tooling::imgui::layout::text_wrapped("Platforms block movement and provide grounded state");
        tooling::imgui::layout::text_wrapped("Hazards reset the player to the Player Start marker");

        tooling::imgui::layout::spacing(2);

        tooling::imgui::layout::text_wrapped("Artillery");
        tooling::imgui::layout::text_wrapped("A/D keys control the angle left and right");
		tooling::imgui::layout::text_wrapped("W/S keys control the power up and down");
        tooling::imgui::layout::text_wrapped("Space fires a projectile");

        tooling::imgui::layout::spacing(2);

        tooling::imgui::layout::text_wrapped("Objects");
        tooling::imgui::layout::text_wrapped("CTRL+Arrow keys move the selected object or selected objects");
        tooling::imgui::layout::text_wrapped("Hold Shift for larger movements");
        tooling::imgui::layout::text_wrapped("The Selected tool palette actions clone or delete the selected editable object");
        tooling::imgui::layout::text_wrapped("Delete or Backspace deletes selected deletable objects");
        tooling::imgui::layout::text_wrapped("CTRL+D clones the active selected object");
        tooling::imgui::layout::text_wrapped("Left click selects one object");
        tooling::imgui::layout::text_wrapped("Shift+Left click toggles objects in the selection set");
        tooling::imgui::layout::text_wrapped("Empty viewport click clears the selection set");
        tooling::imgui::layout::text_wrapped("Use the Select/Move/Scale tool buttons in the top right of the viewport to change tool mode");
        tooling::imgui::layout::text_wrapped("Select tool: left click selects, Shift+Left click toggles selection, and selection handles move selected objects");
        tooling::imgui::layout::text_wrapped("Move tool: left drag on an object handle to move the object");

        tooling::imgui::layout::spacing(2);

        tooling::imgui::layout::text_wrapped("Camera");
        tooling::imgui::layout::text_wrapped("Middle mouse drag: Pan editor camera");
        tooling::imgui::layout::text_wrapped("IJKL keys move the editor camera");
    }
}
