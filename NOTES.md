# Current Phase — Short-term editor/runtime hardening

## Goal

Turn the current editor/runtime foundation into something that behaves more like a typical game engine/editor.

The previous phase proved the important foundations: editor commands, undo/redo, multi-select, explicit tool modes, scale, and the first audio/event hook path. The next phase should now harden the everyday editor behaviour around saving, reset/reload, ordering, and scene state tracking.

This phase is not about adding new systems. It is about removing rough edges that will be awkward to deal with later.

Prune should know when a scene has changed, what can be saved, what can be reset, what order objects render in, and whether runtime state is being viewed or rebuilt. These behaviours should not be inferred accidentally from object shape, current UI selection, or temporary scene runtime state.

## Targets

### 1. Dirty state tracking

Track whether the current scene has unsaved editor-authored changes.

Initial scope:

* [ ] Mark scene dirty when editor commands are executed.
* [ ] Mark scene dirty when inspector edits are committed.
* [ ] Mark scene dirty when authored objects are created, deleted, duplicated, moved, scaled, or reordered.
* [ ] Clear dirty state after a successful save.
* [ ] Keep runtime-only behaviour out of dirty tracking.
* [ ] Show dirty state somewhere lightweight in the editor UI.

Out of scope for the first pass:

* Save confirmation modal when closing Prune.
* Save confirmation modal when switching scenes.
* Per-scene dirty tracking across multiple open scenes.
* Autosave.

### 2. CTRL-S save shortcut

Add a normal editor save shortcut once dirty state exists.

Initial scope:

* [ ] CTRL-S saves the current scene when a scene file path is known.
* [ ] CTRL-S uses the same save path as the existing save workflow.
* [ ] Successful save clears dirty state.
* [ ] Failed save leaves dirty state unchanged.
* [ ] Avoid triggering save repeatedly while the key is held.

Out of scope for the first pass:

* Save As shortcut.
* Recent files.
* Save failure recovery UI beyond a simple message/log entry.

### 3. Runtime reset and reload

Add explicit runtime reset/reload behaviour rather than relying only on pause/resume.

Initial scope:

* [ ] Reset runtime state for the active scene without destroying authored scene data.
* [ ] Reload the scene from saved data when a backing file exists.
* [ ] Preserve the distinction between editor-authored objects and runtime-only objects.
* [ ] Clear runtime-only objects during reset/reload.
* [ ] Re-enter scene runtime cleanly after reset/reload.
* [ ] Make behaviour consistent across Simple Shooter, Platformer, and Artillery.
* [ ] New toolbar in the editor for reset/reload/pause/resume.

Out of scope for the first pass:

* Save prompts before reload.
* Runtime state snapshots.
* Gameplay rewind.

### 4. Z-index ordering

Add authored object ordering so render order is deliberate and editable.

Initial scope:

* [ ] Add an authored z-index/order field to scene objects.
* [ ] Render authored objects using explicit ordering.
* [ ] Persist ordering in scene save data.
* [ ] Restore ordering on load.
* [ ] Add basic editor actions for moving selected object up/down in order.
* [ ] Keep runtime-only ordering behaviour simple and deterministic.

Out of scope for the first pass:

* Full layer system.
* Separate render/collision layers.
* Drag-and-drop ordering in the outliner.

## Follow on development targets

### Medium term, in no particular order

* Rotate tool
* Background image support
* Simplified asset management
* Text rendering
* Behaviour toggles for authored objects
* Polished sample scenes
* Event management and event-driven reactions (play audio, spawn objects, play animation, screen effects, etc.)
* Asset browser
* Animated sprites/facing support
* Scene file versioning
* Grid snapping support on all tools and UI option, not in settings
* Object locking and protection
* More robust collision shapes and collision options
* Scene layering, rendering and collision etc.

### Long term, in no particular order

* Grouping
* Input mapping and rebinding
* Scene settings panel for background, music, and other scene-wide options
* Game UI panels
* Pathfinding support
* Full audio mixer
* Prefabs/templates
* Native playable export
* WebAssembly playable export
* Card scene
* Puzzle scene
* Many more scene tools
