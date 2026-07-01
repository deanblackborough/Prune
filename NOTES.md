# Current Phase — Immediate hardening before next tools

## Goal

Make Prune feel like a real editor rather than a set of game type scene slices with panels attached.

All the previous development proved that multiple scene types can share the same editor/runtime while owning their own behaviour, tools, panels, inspectors, object semantics, defaults, and save data.

During the next development phase, we should now prove that the editor can safely manipulate scene-owned objects without bypassing those scene rules.

## Guiding rule

Editor tools must act through explicit editor semantics, not through incidental object shape.

A selected object is not just a rectangle. It may be authored, runtime-created, selectable, editable, movable, persistent, solid, hazardous, scene-specific, or protected.

## Must do first

- [x] Keep player/controller ownership scene-specific unless a genuinely shared abstraction appears.
- [x] Introduce an editor command/change model before implementing undo/redo.
- [x] Keep undo/redo editor-only at first.
- [x] Keep scene activation separate from default scene creation so loading cannot be overwritten by `on_enter()`.

## Phase targets

### 1. Editor command/change model

Create the smallest useful model for editor-authored changes.

Initial command candidates:

- [x] Move object in scene
- [x] Create object
- [x] Delete object
- [x] Rename object
- [x] Change object size
- [x] Change object rendering type (rectangle/colour)
- [x] Change object colour
- [x] Change object sprite
- [x] Viewport movement/zoom (no zoom yet)
- [x] Record state snapshots not delta changes.

Avoid runtime/gameplay commands for now. A projectile moving, an enemy spawning, or an artillery round ending should not enter editor undo history, we just care about what the user did in the editor.

### 2. Undo/redo

Build undo/redo on top of the command model, not as custom reversal logic scattered through tools and panels.

Initial scope:

- [x] Command history buffer panel with list of past commands
- [x] Move transform tool from scene/tools to editor/tools
- [x] Viewport object movement
- [x] Object creation
- [x] Object deletion
- [x] Simple inspector edits

Out of scope for the first pass:

- Runtime object history
- Gameplay state rewind
- Save/load history restoration
- Multi-scene history

### 3. Delete and duplicate workflow

Add normal editor actions once command history exists.

Initial scope:

- [x] Delete selected authored object
- [x] Duplicate selected authored object
- [x] Select the duplicated object
- [x] Offset duplicates slightly so the result is visible

This gives undo/redo useful behaviour to prove.

### 4. Multi-select

Add multi-select after the single-object command path is stable.

Initial scope:

- [x] Selection set instead of one selected id
- [x] Shift-click to add/remove from the selection set
- [x] Clear selection on empty viewport click
- [x] Outliner multi-select support
- [x] Visible outline for every selected object
- [x] Combined selection bounds in the viewport
- [x] Multi-selection drag handle on the selection bounds
- [x] Selection count in editor UI
- [x] Move selected objects as one editor command
- [x] Delete selected deletable objects as one editor command

Multi-select now supports grouped viewport move and grouped delete. Grouped inspector/property edits remain deferred because they need more specific UX and command labels.

### 5. Tool mode state

Status: implemented.

Introduce an explicit editor tool mode before adding more viewport tools.

Suggested initial tools:

```cpp
enum class EditorTool {
    Select,
    Move,
    Scale,
    Rotate
};
```

This avoids hiding editor behaviour inside whichever handle happened to be clicked.

Current implementation:

- [x] Active tool state lives in generic world scene state.
- [x] A fixed viewport palette exposes Select and Move as text tool buttons.
- [x] The viewport palette also exposes scene-owned creation actions such as Wall, Platform, Hazard, and Terrain Line.
- [x] Select keeps the existing selection and move-handle behaviour.
- [x] Move allows direct body dragging for movable authored objects.
- [x] Scale and rotate remain unimplemented until their interaction rules are explicit.

### 6. Scale tool

Implement scale before rotate.

Scale fits the current object model because Prune already has width/height and rectangle bounds.

Initial scope:

- [x] Single selected authored object
- [x] Corner or edge handle
- [x] Minimum size clamp
- [x] Inspector updates immediately
- [x] Undo/redo command recorded when the drag completes

### 7. Basic audio and event hooks

Add audio through events.

Preferred direction:

- Scenes emit lightweight event ids such as `player_fired`, `enemy_destroyed`, `player_hit`, `round_reset`.
- A small audio layer maps event ids to sounds.
- Scene behaviour does not know how sound is played.

Initial scope:

- [x] One or two hard-coded sound resources
- [x] Fire event
- [x] Hit/destroy event
- [x] Global enable/disable toggle

## Short term, in no particular order

- Z-Index ordering
- Runtime reset and reload, not just pause/resume
- CTRL-S to save scene
- Dirty state tracking for scene

## Medium term, in no particular order

- Rotate tool
- Background image support
- Simplified asset management
- Text rendering
- Behaviour toggles for authored objects
- Polished sample scenes
- Event management and event-driven audio - Event (player_fires), Play sound, Play Animation, Trigger UI effect (screen shake) Visual (play animation),  etc.)
- Asset browser
- Animated sprites/facing support
- Scene file versioning
- Grid snapping support on all tools and UI option, not in settings
- Object locking and protection
- More robust collision shapes and collision options
- Scene layering, rendering and collision etc.

## Long term, in no particular order

- Grouping
- Input mapping and rebinding
- Scene settings panel for background, music, and other scene-wide options
- Game UI panels 
- Pathfinding support
- Full audio mixer
- Prefabs/templates
- Native playable export
- WebAssembly playable export
- Card scene
- Puzzle scene
- Many more scene tools
