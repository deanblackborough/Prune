# Current Phase — Immediate hardening before next tools

## Goal

Make Prune feel like a real editor rather than a set of game type scene slices with panels attached.

All the previous development proved that multiple scene types can share the same editor/runtime while owning their own behaviour, tools, panels, inspectors, object semantics, defaults, and save data.

During the next development phase, we should now prove that the editor can safely manipulate scene-owned objects without bypassing those scene rules.

## Guiding rule

Editor tools must act through explicit editor semantics, not through incidental object shape.

A selected object is not just a rectangle. It may be authored, runtime-created, selectable, editable, movable, persistent, solid, hazardous, scene-specific, or protected.

## Must do first

- Keep player/controller ownership scene-specific unless a genuinely shared abstraction appears.
- Introduce an editor command/change model before implementing undo/redo.
- Keep undo/redo editor-only at first.

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

The first implementation should not include grouped movement or grouped object mutation.

### 5. Tool mode state

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

### 6. Scale tool

Implement scale before rotate.

Scale fits the current object model because Prune already has width/height and rectangle bounds.

Initial scope:

- Single selected authored object
- Corner or edge handle
- Minimum size clamp
- Inspector updates immediately
- Undo/redo command recorded when the drag completes

### 7. Basic audio and event hooks

Add audio through events.

Preferred direction:

- Scenes emit lightweight event ids such as `player_fired`, `enemy_destroyed`, `player_hit`, `round_reset`.
- A small audio layer maps event ids to sounds.
- Scene behaviour does not know how sound is played.

Initial scope:

- One or two hard-coded sound resources
- Fire event
- Hit/destroy event
- Global enable/disable toggle

## Later

- Rotate tool
- Grouping
- Copy/paste
- Prefabs/templates
- Asset browser
- Animation/facing support
- Scene file versioning
- Full audio mixer
- Many more tools
- Card scene
- Puzzle scene
- More robust collision shapes and collision options
