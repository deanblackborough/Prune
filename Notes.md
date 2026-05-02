# Short-term roadmap for editor development

## Step 1 – quick cleanup first

Before adding anything new, tidy a couple of small things that will just get annoying later:

Stop hardcoding 16 everywhere, define a default size properly
Validate sprite keys on load so things don’t silently fail
Maybe group this into a small constants file or similar

## Step 2 – object dragging in the scene

This is the main bit.

What I want:
Click to select (already works)
Click + hold on selected object → start dragging
Mouse movement moves the object in world space
Movement uses proper world coords, not screen hacks
Grid snapping can apply while dragging (optional for now)
Probably don’t allow dragging the player yet, or at least be careful with it
Why this next

Right now everything is inspector-driven. That’s fine for testing, but it’s not how an editor should feel.

Dragging objects in the scene:

makes it immediately more usable
properly tests camera + coordinate handling
sets the base for gizmos later

This feels like the first “this is actually an editor” moment.

## Step 3 – small gameplay slice

Once dragging is in, then it’s worth adding a tiny bit of gameplay.

Keep it simple:

Player can face a direction
Player can shoot something basic
One enemy that moves towards the player
Simple collision (bullet kills enemy)

This isn’t about building a game yet, just stress testing the setup.

I expect this will highlight where the current structure starts to break down.

## Step 4 – scene-specific stuff (later)

After the gameplay slice, then look at:

Splitting scene, it is doing too much right now.

Maybe, Scene, Scene Objects, Scene Renders, Scene Tools, Scene Behaviours, or something like that.

splitting scene types properly (top-down, platformer, etc.)
pulling behaviour out of SandboxScene
introducing scene-specific tooling

Doing this now would just be guessing. Better to wait until the gameplay slice exposes what’s actually needed.

## Misc fixes

- Sprites need to rotate/switch based on player direction
- We are duplicaing 16 everywhere, should be a constant
- Validate sprites against the library when loading (don’t just fail silently)

## What comes next?

- Proper transform gizmos (translate, rotate, scale, axis handles)
- Scene-specific tools (different controls per scene type)
- Behaviour system (move logic out of SandboxScene)
- Sprite system expansion (animation, facing, states)
- Simple collision system (solid objects, triggers, interactions)
- Input abstraction (decouple editor input from gameplay input)
- Camera improvements (zoom, bounds, smoothing, multiple cameras)
- Undo/redo system (editor usability baseline)
- Object duplication & multi-select (basic editor workflow)
- Prefab-style objects (reusable setups, not full system yet)
- Save/load iteration (stability, versioning later)
- Debug overlays (collision, bounds, state visualisation)
- Basic UI for games (text, simple HUD elements)
- Audio hooks (very light, just enough for feedback)
- Scene switching / multiple scenes
- Plugin-style scene architecture (tools + behaviour per scene type)
- Asset handling pass (still simple, no heavy pipeline)
- Performance pass (only once systems exist)
- Packaging / build flow (run outside editor cleanly)
- Documentation / examples (show intent of the project)