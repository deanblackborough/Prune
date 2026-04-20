# Short-term roadmap for editor development

## Step 1: Proper scene viewport

Right now everything renders straight to the SDL window and all input assumes the full window is the scene. That was fine for getting cameras working, but it’s already starting to get in the way.

What’s wrong with the current setup:

screen_to_world() assumes full window space
Picking is based on raw screen coords
UI panels are just drawn over the top, not actually separate
No concept of “this part of the screen is the scene”

What I want instead:

A dedicated viewport panel inside the editor UI
The scene renders into that, not the whole window
Input only affects the scene when the viewport is hovered/focused

Why this matters:

Stops clicks leaking through UI
Gives me a clean place to handle zoom later
Required for gizmos to make sense
Forces a proper separation between editor UI and world interaction

This feels like the right foundation before adding anything more visual or interactive.

## Step 2: Move away from “rectangle + flags”

Current GameObject is basically:

transform
velocity
rectangle render
a couple of flags (solid, is_player, etc.)

That’s already starting to feel too limiting.

What I actually need:

A clear object type/kind (not just booleans)
A render type (start with rectangle + sprite)
Separate “what it is” from “how it looks”
Stop using flags like is_player as identity

Rough direction:

enum or tag for object kind
enum for render type
split visual data from behaviour data

Why do this now:

Sprite support will be awkward without it
Inspectors become cleaner when types are explicit
Save/load will be much easier to reason about

Basically, this is me putting a bit of structure in before things get messy.

## Step 3: Static sprite support

At the moment everything is just coloured rectangles. That’s fine for logic, but visually it’s still very “prototype”.

I don’t want to jump straight into animation, but I do want to support sprites.

Current limitations:

Rendering is just SDL_RenderFillRect
No asset concept
Serialization only knows about colour + size

What I need to add:

Asset path on objects (for sprites)
A simple texture cache/manager
A sprite render path alongside rectangle rendering
Inspector option to switch between rectangle and sprite

Why this step:

Makes the project look like more than a sandbox
Forces me to introduce assets properly
Sets up for animation later without overcommitting

## Step 4: Basic gizmos (movement only)

Right now editing is mostly:

Inspector fields
Add/clone/delete
Keyboard nudging

That works, but it doesn’t feel like an editor yet.

I need to be able to interact with objects directly in the scene.

What I’m aiming for:

Click to select in the viewport
Drag to move objects
Optional grid snapping while dragging
Visual feedback (bounds/selection highlight)

Deliberately not doing yet:

Scaling
Rotation

Those can come later once movement feels solid.

This step is mostly about making the editor feel usable rather than just functional.

## Step 5: Start separating scene-specific tooling

At the moment everything is effectively hardcoded into one flow:

SandboxScene
One UI render path
One inspector/outliner setup
One object model

That’s fine early on, but it doesn’t match the longer-term goal.

I want different scene types (platformer, top-down, etc.), each with their own tools and inspectors.

So I need to start untangling things before it grows further.

What I want to introduce:

Scene-level hooks for editor behaviour (not just update/render)
Ability for scenes to define their own inspector sections
Scene-specific outliner/tool behaviour
Scene-aware save/load

At the same time:

Keep shared systems generic (camera, viewport, selection, object manager helpers)

Important constraint:

Don’t over-engineer this
Don’t build a full framework yet
Just stop SandboxScene becoming the permanent dumping ground