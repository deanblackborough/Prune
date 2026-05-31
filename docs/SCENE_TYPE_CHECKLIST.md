# Scene type checklist

Use this when adding or reviewing a scene type.

The goal is not to create a plugin API. The goal is to keep each scene responsible only for what makes it different.

## Required scene-owned pieces

A scene type should define:

- Scene type id
- Scene label
- Default scene file path
- Scene-specific state
- Scene-specific behaviour
- Scene-specific concepts/object meanings
- Scene-specific factories/default objects
- Scene-specific save/load data
- Scene-specific inspector section
- Scene-specific tool/panel content where needed

## Shared editor/runtime pieces it should reuse

A scene type should not reimplement:

- Viewport ownership
- Editor camera foundations
- Game camera foundations
- Grid rendering
- Generic object rendering
- Generic object selection flow
- Generic inspector shell
- Outliner shell
- Basic save/load wrapper flow
- View-centre object placement helper
- Runtime object protection rules

## Object semantics checks

For each authored object type, define:

- What is this object called in this scene?
- What is its purpose?
- Is it selectable?
- Is it editable?
- Is it movable?
- Is it persistent?
- Is it solid?
- What are its collision rules?
- Is colour only presentation, or is behaviour incorrectly depending on it?

For each runtime object type, define:

- What creates it?
- What destroys it?
- Should it appear in the outliner?
- Should it be selectable?
- Should it ever be saved?
- How should debug overlays identify it?

## Input checks

A scene type should be clear about:

- Which input belongs to gameplay?
- Which input belongs to editor tools?
- What happens when the UI owns keyboard input?
- What happens when the UI owns mouse input?
- What happens when the scene is paused?
- Can the editor camera and game camera both be used safely?

## Save/load checks

Before calling a scene type stable enough for the prototype:

- New scene works.
- Save works.
- Load works from the same scene type.
- Load works when another scene type is currently active.
- Runtime objects are not saved.
- Scene-specific state survives round trip.
- Selection is valid after load.
- Camera state is sensible after load.
- Invalid/missing object ids are repaired or reset.

## Tooling checks

Before generic editor tools act on a scene object, check:

- The object is selectable.
- The object is editable.
- The object is movable if the tool moves it.
- The object is not runtime-only unless explicitly allowed.
- The action should be recorded by undo/redo.
