# Current Phase — Short-term editor/runtime hardening

## Goal

Turn the current editor/runtime foundation into something that behaves more like a typical game engine/editor.

The previous phase proved the important foundations: editor commands, undo/redo, multi-select, explicit tool modes, scale, and the first audio/event hook path. This phase has now added dirty-state tracking, Ctrl+S, shared runtime Reset/Pause/Play controls, and authored-object render ordering (z-index). The short-term hardening targets are complete.

This phase is not about adding new systems. It is about removing rough edges that will be awkward to deal with later.

Prune should know when a scene has changed, what can be saved, what can be reset, what order objects render in, and whether runtime state is being viewed or rebuilt. These behaviours should not be inferred accidentally from object shape, current UI selection, or temporary scene runtime state.

## Targets

### 1. Dirty state tracking

Track whether the current scene has unsaved editor-authored changes.

Initial scope:

* [x] Mark the scene dirty when a command modifies persistent, editor-authored scene data.
* [x] Mark scene dirty when inspector edits are committed.
* [x] Mark scene dirty when authored objects are created, deleted, duplicated, moved, scaled, or reordered.
* [x] Clear dirty state after a successful save.
* [x] Keep runtime-only behaviour out of dirty tracking.
* [x] Show dirty state somewhere lightweight in the editor UI.

Out of scope for the first pass:

* Save confirmation modal when closing Prune.
* Save confirmation modal when switching scenes.
* Per-scene dirty tracking across multiple open scenes.
* Autosave.

### 2. CTRL-S save shortcut

Add a normal editor save shortcut once dirty state exists.

Initial scope:

* [x] CTRL-S saves the current scene when a scene file path is known.
* [x] CTRL-S uses the same save path as the existing save workflow.
* [x] Successful save clears dirty state.
* [x] Failed save leaves dirty state unchanged.
* [x] Avoid triggering save repeatedly while the key is held.

Out of scope for the first pass:

* Save As shortcut.
* Recent files.
* Save failure recovery UI beyond a simple message/log entry.

### 3. Runtime reset, pause, and play

Add explicit runtime reset behaviour and shared Pause/Play controls.

Initial scope:

* [x] Reset runtime state for the active scene without destroying authored scene data.
* [x] Use the current in-memory authored state as the reset baseline, including edits made while the runtime is live.
* [x] Preserve the distinction between editor-authored objects and runtime-only objects.
* [x] Clear runtime-only objects during reset.
* [x] Re-enter scene runtime cleanly and continue playing after reset.
* [x] Make behaviour consistent across Simple Shooter, Platformer, and Artillery.
* [x] Add Reset and the currently relevant Pause or Play action to the shared editor toolbar.
* [x] Remove the previous scene-specific pause controls from the UI.
* [x] Keep Reset, Pause, and Play as non-dirty editor actions.
* [x] Use word buttons at the top of the existing mode toolbar until the final icon treatment is designed.

Out of scope for the first pass:

* Runtime state snapshots.
* Gameplay rewind.

### 4. Z-index ordering

Add authored object ordering so render order is deliberate and editable.

Initial scope:

* [x] Add an authored z-index/order field to scene objects.
* [x] Render authored objects using explicit ordering.
* [x] Persist ordering in scene save data.
* [x] Restore ordering on load.
* [x] Add basic editor actions for moving selected object up/down in order.
* [x] Keep runtime-only ordering behaviour simple and deterministic.

Out of scope for the first pass:

* Full layer system.
* Separate render/collision layers.
* Drag-and-drop ordering in the outliner.

## Follow on development targets

### Build, portability, and CI

Prune currently only builds and is verified on Windows/MSVC. Portable, sanitizer-clean C++ is a credibility signal worth having, and Linux-first contributors cannot try Prune at all today.

* Build and run on Linux with both GCC and Clang, alongside the existing Windows/MSVC build.
* Extend CI to a compiler matrix (GCC, Clang, MSVC).
* Add an AddressSanitizer + UndefinedBehaviorSanitizer build and run the test suite under it in CI.
* Add a scripted headless run of the app under sanitizers (needs a smoke/headless mode or a dummy SDL video driver).
* Add a `.clang-format` config enforced in CI, and a `clang-tidy` pass.
* Work through the portability issues the Linux port surfaces (`std::filesystem` path handling, SDL include paths, any MSVC-isms).
* Document object lifetime and pointer-validity discipline (ids as the durable handle, pointers as frame-local) as an ADR.

### Medium term, in no particular order

* Rotate tool
* Background image support
* Simplified asset management
* Text rendering
* Behaviour toggles for authored objects
* Polished sample scenes
* Authored event management and event-driven reactions (configure the existing audio path, spawn objects, play animation, screen effects, etc.)
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
