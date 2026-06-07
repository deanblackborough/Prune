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

        tooling::imgui::layout::text_wrapped("Objects");
        tooling::imgui::layout::text_wrapped("CTRL+Arrow keys move the active selected object");
        tooling::imgui::layout::text_wrapped("Hold Shift for larger movements");
        tooling::imgui::layout::text_wrapped("Delete or Backspace deletes the active selected object");
        tooling::imgui::layout::text_wrapped("CTRL+D duplicates the active selected object");
        tooling::imgui::layout::text_wrapped("Left click selects one object");
        tooling::imgui::layout::text_wrapped("Shift+Left click toggles objects in the selection set");
        tooling::imgui::layout::text_wrapped("Empty viewport click clears the selection set");
        tooling::imgui::layout::text_wrapped("Mouse button left on the active selected handle moves the object");

        tooling::imgui::layout::spacing(2);

        tooling::imgui::layout::text_wrapped("Camera");
        tooling::imgui::layout::text_wrapped("Middle mouse drag: Pan editor camera");
        tooling::imgui::layout::text_wrapped("IJKL keys move the editor camera");
    }
}
