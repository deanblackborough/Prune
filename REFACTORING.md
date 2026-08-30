# Architecture Refactoring — Authoring model and runtime/editor boundary

## Goal

Make the seam between **authored state**, **live runtime state**, and **editor/session state**
explicit and enforced, before rotation, layers, behaviour toggles, authored events, grouping,
and prefabs start relying on it.

This is a contained refactoring phase, not a redesign. The scene ownership model, `WorldScene`
as the shared foundation, explicit factories, scene-specific serializers, durable IDs, snapshot
undo, and `ObjectConcept` all stay. See *Guiding constraints* below.

## Why now

Prune has reached its first real refactoring boundary. The current design proved the scene
slices, but three decisions that were fine for proving them are now structural debt:

- Persistent scene settings (gravity, speeds, cooldowns, aim/power) are mutated directly in
  tooling and never pass through commands, dirty tracking, or undo/redo.
- `WorldScene` keeps a live object collection and a separate authored collection, and every
  command is interpreted twice to keep them in sync (`normalize_editor_command`,
  `apply_editor_command_to_authored_objects`, `apply_authored_delta`).
- `EditorCommand` is a type enum plus a bag of optional/vector fields whose validity depends
  on the enum. Roughly nine of the fourteen command types are mechanically identical.

Adding rotation, then layers, then behaviours on top of this makes each of the three worse.
Fix the authoring model first (Phase A), then the runtime/editor boundary (Phase B).

## How to use this document

Work top to bottom. Phase 0 is a short build/CI pre-flight that de-risks the rest. Phase A
items are ordered by dependency and should be done in order. Phase B items can be done in any
order once Phase A is stable. Phase C items are feature-triggered — do them when the feature
that needs them lands.

Tick the checkboxes as items complete. Each item is written so it can be picked up cold.

---

## Guiding constraints

Keep these unless a concrete requirement proves them insufficient:

- `WorldScene` as the shared world/editor base. The hierarchy is shallow and meaningful.
- Explicit `SceneFactory` switch and static `SceneDescriptor`. No registries, no dynamic
  scene registration.
- Scene-specific serializers. No generic reflective serializer.
- Scene-owned behaviours (Platformer / Simple Shooter / Artillery stay concrete slices).
- `ObjectConcept` as the editor-facing description of scene semantics.
- Durable `GameObjectId` handles, frame-local pointer access.
- Snapshot-based command history. No delta compression, no inverse-operation machinery.

Do **not** introduce, as part of this work:

- dependency injection containers, service locators, generic event buses;
- reflection systems, plugin systems;
- abstract repositories wrapping `std::vector`;
- an ECS. The problems here are not caused by the absence of one.

The objective is stronger ownership and dependency boundaries, not more abstraction.

---

# Phase 0 — Pre-flight

These are the parts of the NOTES.md "Build, portability, and CI" work whose value is highest
*before and during* the authoring refactor: they are the safety net for A2/A3, and doing them
first keeps the refactor diffs clean. The rest of that NOTES.md section stays where it is —
the scripted headless sanitizer run is blocked until B2/B5, and the clang-tidy pass is better
done after the architecture settles.

Do not treat Phase 0 as a gate on everything: P0.1 and P0.2 should land before A1, P0.3 and
P0.4 can run in parallel with early Phase A.

## P0.1. `.clang-format`, applied tree-wide

### Why now

A one-commit reformat before A1–A4 keeps every refactor diff reviewable. Introducing it
mid-refactor mixes formatting churn into structural changes.

### Checklist

- [x] Add a `.clang-format` matching the current house style as closely as possible.
- [ ] Apply it to the whole tree in a single isolated commit.
- [x] Enforce it in CI (fail on unformatted diff).

## P0.2. Sanitizer build + tests under it in CI

### Why now

A3 reworks the authored/live projection and the id-as-handle vs pointer-access split — exactly
the class of bug ASan/UBSan catch. This must be running before that code moves, not after.

### Checklist

- [ ] Add an ASan + UBSan build configuration.
- [ ] Run the existing test suite under it in CI.
- [ ] Fix anything it flags in the current code before starting A1.

## P0.3. Second compiler / Linux build

### Why now

MSVC is lax about lifetime and template issues that GCC/Clang reject. Catching those per-PR
during the refactor is worth the upfront port cost. The port is mostly build-system plus
isolated fixes (`std::filesystem` paths, SDL include paths, MSVC-isms) and is largely
orthogonal to the architecture work.

If clean "did my refactor break this" attribution matters, finish this before starting A1 so
Phase A begins from a green cross-platform baseline.

### Checklist

- [ ] Build with Clang (Windows or Linux) alongside MSVC.
- [ ] Build and run on Linux with GCC and Clang.
- [ ] Work through the portability issues the port surfaces.
- [ ] Extend CI to the compiler matrix (GCC, Clang, MSVC).

## P0.4. Object lifetime / pointer-validity ADR

### Why now

This ADR *is* the formalisation of the rule A3 implements (`GameObjectId` is the durable
handle, `GameObject*` is frame-local). Write it alongside A3, not separately.

### Checklist

- [ ] Draft the ADR as part of A3.
- [ ] Cross-reference it from A3 and from DECISIONS.md.

### Deferred (not Phase 0)

- **Scripted headless run of the app under sanitizers** — blocked until B2 removes ImGui from
  the runtime scene layer and B5 gives a `prune_runtime` target. Tracked in NOTES.md.
- **clang-tidy pass** — better after the architecture settles, so lint effort is not spent on
  code A2/A3 delete or move.

---

# Phase A — Fix the authoring model

Do not start substantial new feature work until A1–A4 are done and A5 covers them.

## A1. Separate authored scene configuration from transient runtime state

### Problem

Each scene mixes authored configuration and transient runtime state in one options struct,
and the serializer writes whichever the live struct currently holds. Pause is defined as a
non-dirty action, but `PlatformerScene::set_runtime_paused` writes `options.paused` and
`PlatformerSerializer::save_to_node` persists it. Edit an object, pause, save — the save file
now records `paused: true` even though pausing was never an authored change.

The generic serializer also writes editor/session state into the save file that should not be
there: `scene.selected_object_id`, the editor camera pose, `options.debug_overlays.*`, and
(implicitly) editor view state.

### Current state

- `src/prune/scene/platformer/platformer_state.hpp` — `PlatformerOptions { paused, move_speed,
  jump_velocity, gravity, max_fall_speed }`, plus `player_grounded` on `PlatformerState`.
- `src/prune/scene/simple_shooter/simple_shooter_state.hpp` — `SimpleShooterOptions`; player
  speed lives separately in `player_controller`.
- `src/prune/scene/artillery/artillery_state.hpp` — `ArtilleryOptions`, plus `player_one_aim`
  / `player_two_aim` (runtime-adjusted, with an ad-hoc `m_authored_*_aim` baseline held in the
  scene), plus `current_turn`, `projectile_active`.
- `src/prune/scene/*/*_serializer.cpp` — write the whole options struct including `paused`.
- `src/prune/scene/scene_serializer.cpp` — writes `scene.selected_object_id`,
  `cameras.editor.*`, `options.debug_overlays.*`.

### Target

Two structs per scene: authored settings (serialized, participates in dirty/undo later in A4)
and transient runtime state (never serialized, rebuilt on reset).

Decisions taken:

- **All camera state leaves the save file** — both the editor camera pose and the game camera.
  Game start framing (zoom, position, follow target) is established by the scene on `new_scene`
  *and* on load, from the scene factory / `restore_defaults`-style logic, not from the file.
  A follow camera overrides its position on the first frame anyway, and the scenes already set
  their framing in `restore_defaults()` today.
- **Grid options are an editor session preference**, not per-scene save data. This matches the
  NOTES.md intent to move grid snapping to a UI option rather than scene settings.
- **Current selection, debug overlays, active editor tool** are editor session state — remove
  from the save file.
- `scene.next_object_id` stays — it is required for stable ID allocation across sessions.

### Checklist

- [ ] Platformer: split into `PlatformerSettings { move_speed, jump_velocity, gravity,
      max_fall_speed }` and `PlatformerRuntime { bool paused, bool player_grounded }`.
- [ ] Simple Shooter: `SimpleShooterSettings` (including player speed, moved out of
      `player_controller` or exposed from it as authored config) and a runtime struct.
- [ ] Artillery: `ArtillerySettings { gravity, min_power, max_power, initial_p1_aim,
      initial_p2_aim }` and `ArtilleryRuntime { current_turn, projectile_active, live p1/p2
      aim }`. Drop the ad-hoc `m_authored_*_aim` members once settings hold the initial aims.
- [ ] Scene serializers write only the authored settings struct. Remove `paused` and any other
      runtime value from the persisted node.
- [ ] `reset_runtime()` rebuilds the runtime struct from defaults + authored settings.
- [ ] Generic serializer: stop writing `selected_object_id`, the whole `cameras` section, and
      `options.debug_overlays.*`. Keep `objects` and `scene.next_object_id`.
- [ ] `load_from_node` no longer requires a `cameras` section; drop that from the required-keys
      check.
- [ ] Each scene establishes game camera framing on load (not only on `new_scene`) — move the
      framing setup so both paths call it.
- [ ] Move grid options off `SceneState` / save into an editor session location.
- [ ] Loading an old save file that still contains the removed keys must not error — ignore
      unknown keys.

### Out of scope

- Scene file versioning (tracked separately in NOTES.md).
- A migration tool for existing save files — ignoring removed keys is enough for now.

---

## A2. Redesign `EditorCommand` around typed payloads

### Problem

`EditorCommand` is `EditorCommandType type` plus `std::optional<GameObject> before/after`,
`std::vector<GameObject> before/after`, `std::vector<GameObjectId> object_ids`,
`std::optional<Camera> before/after`. Which fields are valid depends entirely on `type`.
`EditorCommand{ .type = MoveViewport, .before_object = obj }` compiles and is meaningless.

Nine of the fourteen types (`MoveObject`, `RenameObject`, `ChangeObjectPosition`,
`ChangeObjectSize`, `ChangeObjectRenderType`, `ChangeObjectColour`, `ChangeObjectZIndex`,
`ChangeObjectFlag`, `ChangeSprite`) carry exactly `before_object` + `after_object` and differ
only by history label and by which `apply_authored_delta` case runs. Rotation would add a
tenth identical type.

Construction already funnels through `make_*` factory helpers, so malformed commands are not
being built today. The cost is on the read side: every consumer switches on `type` and trusts
conventions, and `apply_authored_delta` is a hand-maintained per-field merge that every new
type must extend.

### Current state

- `src/prune/editor/editor_command.hpp` — the struct and the `EditorCommandType` enum.
- `src/prune/editor/editor_command.cpp` — `make_*` factories, `editor_command_type_label`.
- `src/prune/scene/world_scene.cpp` — `apply_editor_command`, `apply_authored_delta`,
  `normalize_editor_command` all switch on `type`.
- `src/prune/tooling/editor/tracked_property_table.*`, `src/prune/scene/scene_interaction.cpp`,
  `src/prune/editor/editor_actions.cpp` — construct commands.

### Target

```cpp
struct ObjectChange   { GameObject before; GameObject after; };   // was: 9 object-edit types
struct ObjectsChange  { std::vector<GameObject> before; std::vector<GameObject> after; };
struct ObjectCreate   { GameObject object; };
struct ObjectDelete   { GameObject object; };
struct ObjectsDelete  { std::vector<GameObject> objects; };
struct ViewportChange { Camera before; Camera after; };
struct SceneSettingsChange { YAML::Node before; YAML::Node after; };   // new, see A4

using EditorCommandPayload = std::variant<
    ObjectChange, ObjectsChange, ObjectCreate, ObjectDelete,
    ObjectsDelete, ViewportChange, SceneSettingsChange>;

struct EditorCommand {
    std::string label;
    std::string detail;
    bool makes_dirty = true;
    EditorCommandPayload payload;
};
```

History label/detail describe the operation for the UI. They do **not** drive replay — a
"Change colour" and a "Move" are both an `ObjectChange`. This is what makes rotation and later
object-editing features cheap: they reuse `ObjectChange`, they do not add a payload type.

`SceneSettingsChange` carries two YAML snapshots of the scene's authored settings section
(reusing the scene serializer). Apply/undo call back into the scene to load the relevant
snapshot. No `std::function`, no per-setting enum — one payload for all scene settings across
all scene types.

### Checklist

- [ ] Define the payload structs and the `std::variant`. Keep `GameObjectId` helpers where
      consumers still need a quick "which objects did this touch" answer (a small
      `affected_ids(const EditorCommand&)` free function over the variant).
- [ ] Rewrite the `make_*` factories to build payloads. Collapse the nine object-edit helpers
      into `make_object_change(label, before, after, detail)`.
- [ ] Replace every `switch (command.type)` with `std::visit` or `std::get_if`.
- [ ] Delete `EditorCommandType` and `editor_command_type_label`.
- [ ] Update `tests/editor_command_history_tests.cpp` and add payload construction coverage.
- [ ] Do this before rotation. Rotation then lands as a label + reuse of `ObjectChange`.

### Out of scope

- Changing the undo/redo cursor mechanics in `EditorCommandHistory` — only the payload shape
  changes.
- Command coalescing (merging rapid successive edits into one history entry).

---

## A3. Make the authored store authoritative

### Problem

`WorldScene` holds `m_state.objects` (live) and `m_authored_objects` (reset/persist baseline).
The live collection is authoritative: edits mutate live objects, then `record_editor_command`
interprets the command a second time to patch the authored copy via `apply_authored_delta`,
a per-field `switch`. Every new persistent field is another case to remember. Miss one and the
authored copy silently drifts from what will be saved.

`normalize_editor_command` then re-reads the current authored object before recording, to make
undo/redo target authored state rather than whatever the live object looked like. This is a
lot of implicit bookkeeping concentrated in one place, and it grows with every field.

### Current state

- `src/prune/scene/world_scene.hpp` / `.cpp` — `m_authored_objects`, `capture_authored_objects`,
  `normalize_editor_command`, `apply_editor_command_to_authored_objects`, `apply_authored_delta`,
  `apply_editor_command`, `restore_object_snapshot(s)`, `restore_object_in_id_order`.
- `reset_runtime()` — `m_state.objects = m_authored_objects` then re-selects and restarts.
- Edit call sites mutate the live `GameObject&` directly, then commit a command.

### Target

The authored store is the source of truth for persistent data. An authored edit updates the
authored store first, then the change is reflected into the live projection.

Decision taken: **authored fields are read-only in the inspector/tooling while the runtime is
playing.** Editing happens against the authored baseline, not against a diverged live world.
This removes the hard case (reconciling an edit made to a mid-play object back into authored)
entirely.

Model:

```
AuthoredWorld   authored objects + authored scene settings   (authoritative, serialized)
RuntimeWorld    live projection: authored instances + runtime-only objects + transient state
EditorSession   selection, drag, active tool, command history, dirty  (see B3)
```

- **Edit flow:** UI edit → `EditorCommand` targeting authored object(s) or settings → apply to
  `AuthoredWorld` → apply the same change to `RuntimeWorld` for the affected IDs.
- One shared `apply_object_change(GameObjectManager&, const ObjectChange&, Direction)` used for
  both the authored store and the live projection. `apply_authored_delta` is deleted.
- **Undo/redo:** inverse-apply to `AuthoredWorld`, then re-project affected IDs into
  `RuntimeWorld`.
- **Reset:** `RuntimeWorld` objects rebuilt from `AuthoredWorld` (as today).
- **Runtime-only objects** (bullets, spawned enemies) live only in `RuntimeWorld`, are never in
  `AuthoredWorld`, and are never the target of an `EditorCommand`.
- `normalize_editor_command` is deleted — commands are authored against authored state by
  construction, because that is the only state editable.

Transitional note: drag feedback can still mutate the live object during the drag for
immediate rendering; on commit, the command applies canonically to `AuthoredWorld` and the
affected IDs are re-projected. The invariant is that `AuthoredWorld` is correct at every commit
boundary, not every frame.

### Edit mode — Stop action

Decision taken: add a **Stop** action.

- **Stop** = rebuild the live world from the authored baseline and hold it paused. This is the
  editable state. Authored fields in the inspector/tooling are editable only when stopped (or
  before the first Play).
- **Play** = run the simulation from wherever it currently is.
- **Pause** = freeze the simulation on exactly the current frame. Does *not* restore the
  baseline, and does *not* enable authored editing.
- **Reset** stays as-is: rebuild from baseline and start playing.

Toolbar shows Stop / Play / Pause according to state. A freshly loaded or newly created scene
starts stopped (editable).

### Checklist

- [ ] Introduce `AuthoredWorld` (can be a struct owning a `GameObjectManager` + a handle to the
      scene's authored settings) as the authoritative store. Promote `m_authored_objects`.
- [ ] `record_editor_command`: apply to `AuthoredWorld`, then project affected IDs into the
      live `GameObjectManager`. Remove `normalize_editor_command`.
- [ ] `undo_editor_command` / `redo_editor_command`: inverse-apply to `AuthoredWorld`, re-project.
- [ ] Single `apply_object_change` / `apply_objects_change` / create / delete helpers, shared
      between authored and live. Delete `apply_authored_delta` and
      `apply_editor_command_to_authored_objects`.
- [ ] Inspector and scene tooling: bind editable fields to the authored object; disable them
      (show live values as read-only info rows) unless the scene is stopped.
- [ ] Add the **Stop** action: rebuild the live world from the authored baseline, hold paused,
      enter edit mode. Wire Stop / Play / Pause into the shared toolbar per state.
- [ ] A freshly loaded or newly created scene starts stopped.
- [ ] Gate authored-field editability on "stopped" rather than "not playing".
- [ ] `restore_object_in_id_order` no longer needs to stable-sort the whole vector on every
      insert once the projection path is centralised — tidy it.
- [ ] `reset_runtime` unchanged in behaviour, but sourced from `AuthoredWorld`.

### Out of scope

- Making `RuntimeWorld` a separate class with its own type. A `GameObjectManager` plus the
  transient runtime struct from A1 is enough for now.
- Runtime state snapshots / rewind.

---

## A4. Route every persistent scene-specific edit through a command

### Problem

Scene tooling edits authored settings directly:

- `src/prune/tooling/platformer/platformer.cpp` — `slider_float` straight onto
  `state.options.gravity`, `move_speed`, `jump_velocity`, `max_fall_speed`.
- `src/prune/tooling/simple_shooter/simple_shooter.cpp` — enemy speed, projectile speed,
  projectile lifetime, fire cooldown, max live enemies; player speed via `player_controller`.
- `src/prune/tooling/artillery/artillery.cpp` — gravity, per-player angle and power. The scene
  copies the values into `m_authored_*_aim` on change but records no command and sets no dirty
  flag.

None of these participate in undo/redo, dirty tracking, or (for Platformer) even the reset
baseline — edited gravity silently survives `reset_runtime()` because nothing captures or
restores it.

The infrastructure for doing this correctly exists for per-object edits
(`ObjectEditTracker` + `tracked_property_table`); scene settings just do not use it, partly
because they are not `GameObject`-shaped. A2 provides the missing payload.

### Target

Every value that can change the saved scene is edited through a `SceneSettingsChange` command:
capture the authored settings snapshot on edit begin, commit on edit end, dirty + undo/redo
for free.

### Checklist

- [ ] Add a scene settings edit tracker analogous to `ObjectEditTracker`: capture the settings
      YAML snapshot when a control becomes active, commit a `SceneSettingsChange` when it
      deactivates after a change.
- [ ] `WorldScene` handles `SceneSettingsChange` by calling a virtual
      `apply_scene_settings_snapshot(const YAML::Node&)` on the scene (reusing the
      A1 authored-settings serializer).
- [ ] Convert Platformer scene tooling to the tracked path.
- [ ] Convert Simple Shooter scene tooling to the tracked path.
- [ ] Convert Artillery scene tooling to the tracked path; delete the ad-hoc `m_authored_*_aim`
      bookkeeping.
- [ ] Confirm: editing a scene setting marks dirty, is in the undo history with a readable
      label, and survives save/load; undo restores the previous value.

### Out of scope

- A dedicated scene settings panel (that is a separate NOTES.md item). This is about the edit
  path, not the UI layout.

---

## A5. Architectural integration tests for the authoring contract

### Problem

`tests/editor_command_history_tests.cpp` is the only test file and only exercises the
undo-cursor mechanics of `EditorCommandHistory`. The authored/live synchronisation logic —
the part most likely to break as fields are added — has no coverage.

### Target

A small number of behavioural integration tests over the authoring layer, runnable without
SDL or ImGui. A3/B2/B5 should make this possible; if the authoring layer cannot be tested
headlessly after A3, that is a signal A3 did not go far enough.

### Checklist

- [ ] Make `AuthoredWorld` + command application testable without a full scene runtime
      (construct a minimal scene or a test double).
- [ ] `edit → runtime mutation → undo` restores the authored value.
- [ ] `edit → runtime mutation → reset` produces the authored value.
- [ ] `edit → save → mutate → undo → save` round-trips correctly.
- [ ] `create → undo → redo → reset`.
- [ ] `delete → runtime continues → undo` restores the object.
- [ ] `multi-move → reset`.
- [ ] `z-index change → undo → save → load`.
- [ ] `scene setting change → dirty → undo → redo → save → load`.
- [ ] `pause → save → load` does **not** persist or restore a paused runtime.
- [ ] Wire into `tests/CMakeLists.txt`.

### Out of scope

- Full end-to-end tests driving the ImGui UI.
- A large quantity of narrow unit tests — prefer a few scenario tests.

---

# Phase B — Strengthen the runtime/editor boundary

Start after Phase A is stable. Items can be done in any order, though B5 depends on B2.

## B1. Give objects an explicit authored concept identity

### Problem

Scene concepts identify objects by their runtime behaviour string:

```cpp
bool is_player(const GameObject& o) { return o.runtime.behaviour == platformer_ids::player_behaviour; }
```

(`src/prune/scene/platformer/platformer_concepts.cpp`, same pattern in the other scenes.)

One string answers two different questions: *what is this authored object* and *what runtime
behaviour does it execute*. Behaviour toggles are on the roadmap — a "Moving Platform" concept
could have `solid`, `moving`, `damage_on_contact`, `event_emitter` behaviours. The concept must
not be inferred from whichever behaviour string happens to be attached.

`ObjectConcept` already carries a `concept_id` string (`"platformer.player"`), but it is
derived from the behaviour-based `kind_for`, not stored on the object.

### Target

An explicit authored concept identifier on the object, set by the factory, used by concepts,
serializers, and restore/validation. Keep it simple:

```cpp
struct ObjectSemantics { std::string concept_id; };   // on GameObject
```

Not a component framework. Not an ECS. Just stop treating `runtime.behaviour` as the type
system.

### Checklist

- [ ] Add `concept_id` (or `ObjectSemantics`) to `GameObject`.
- [ ] Scene factories set it explicitly.
- [ ] `*_concepts.cpp` match on `concept_id`, not `runtime.behaviour`.
- [ ] Serializers persist and load it.
- [ ] Restore/validation logic (`restore_loaded_*_concepts`) keys off it.
- [ ] Old save files without the key: derive `concept_id` from `runtime.behaviour` on load as a
      one-time fallback.
- [ ] Do this before behaviour toggles or authored events.

---

## B2. Move scene-specific ImGui presentation out of runtime scene classes

### Problem

The `Scene` interface exposes `draw_scene_tools`, `draw_viewport_overlays`,
`draw_scene_inspector`. Scene implementations and `WorldScene` itself depend on ImGui and
`tooling/`. Dependency direction is runtime scene layer → editor presentation layer, which
blocks native/WASM playable export, headless and sanitizer smoke testing, and runtime builds
without Dear ImGui.

### Current state

- `src/prune/scene/scene.hpp` — the `draw_*` virtuals.
- `src/prune/scene/world_scene.cpp` — `#include "imgui.h"`, `draw_viewport_overlays`,
  `draw_debug_tools`.
- Every `src/prune/scene/*/*_scene.cpp` — includes ImGui and `tooling/`, implements `draw_*`.

### Target

Keep scene ownership of tools/inspectors/overlays, but split the class:

```
PlatformerScene    authored + runtime Platformer behaviour   (no ImGui)
PlatformerEditor   Platformer inspector, tools, overlays      (owns the ImGui)
```

The editor shell constructs the matching editor extension for the active scene type via a
small explicit factory (mirroring `SceneFactory`). No plugin system, no registration, no DI.

### Checklist

- [ ] Remove `draw_*` from the `Scene` interface.
- [ ] Introduce a `SceneEditor` (name TBD) interface with the presentation methods.
- [ ] One `SceneEditor` implementation per scene type; move the `draw_*` bodies into them.
- [ ] Small explicit factory: scene type → `SceneEditor`.
- [ ] `WorldScene` and scene subclasses drop their ImGui / `tooling/` includes.
- [ ] The editor shell owns the active `SceneEditor` and drives it.

---

## B3. Separate editor/session state from runtime world state

### Problem

`SceneState` (`src/prune/scene/scene_state.hpp`) holds `dirty`, `viewport`, `drag_state`,
`scene_options`, `editor_tool`, `editor_commands` alongside `events` and `objects`. Runtime
behaviour functions receive the whole `SceneState&` (`m_platformer.update(m_state, ...)`), so
gameplay code can currently reach command history, drag state, the active tool, and dirty
state.

### Target

Split what runtime behaviour sees from editor/session state.

```
WorldState        objects, events, transient runtime state    → passed to update_runtime
EditorSceneState  viewport, drag, tool, command history, dirty, selection (see B4)
```

Not many interfaces, not granular state services. Just make the dependency visible and stop
gameplay code from casually depending on editor state.

### Checklist

- [ ] Define `WorldState` (or reuse the A1 runtime struct + `GameObjectManager`).
- [ ] `update_runtime` and scene behaviour take `WorldState&`, not `SceneState&`.
- [ ] Move `dirty`, `drag_state`, `editor_tool`, `editor_commands`, `viewport` into an editor
      session container.
- [ ] `scene_options` / debug overlays go with editor session state.

---

## B4. Move selection out of `GameObjectManager`

### Problem

`GameObjectManager` owns `m_selected_ids` and all `select* / toggle_selected /
selected_object / clear_selection` operations. Selection is editor state, not world simulation
state. This reinforces the coupling between the runtime object container and editor
infrastructure, and there is nowhere natural for grouping / hierarchy / multi-select rules to
evolve.

### Target

```
GameObjectManager   object ownership and storage only
EditorSelection     selected IDs, primary selection, selection operations
```

### Checklist

- [ ] Introduce `EditorSelection`, move the selection state and methods.
- [ ] `GameObjectManager` keeps only storage/access.
- [ ] Update `scene_interaction`, tooling, and `WorldScene` call sites.
- [ ] Do this together with B3.

---

## B5. Introduce a small CMake runtime/editor target split

### Problem

Everything is one `add_executable(Prune ...)` target
(`CMakeLists.txt`). CMake cannot reject a runtime source file that includes editor tooling or
ImGui, so "runtime code must not depend on editor presentation" stays documentation rather
than a build constraint. This item is the enforcement mechanism for B2 — it cannot be done
until B2 removes ImGui from the runtime scene layer.

### Target

```
prune_runtime   runtime + scene behaviour, no Dear ImGui
prune_editor    editor shell, tooling, scene editors, links ImGui
Prune           thin executable
```

Two or three meaningful targets. Not a collection of micro-libraries. Scene code can stay
inside these targets rather than being split further.

### Checklist

- [ ] After B2: create `prune_runtime` with no ImGui link.
- [ ] Move editor/tooling/scene-editor code into `prune_editor`.
- [ ] `Prune` links both; `PruneTests` can link `prune_runtime` alone for headless tests.
- [ ] Verify `prune_runtime` does not transitively pull in ImGui.

---

# Phase C — Feature-triggered

Do these when the feature that needs them is next, not before.

## C1. Distinguish scene-concept policy from per-object authored locks

Editability is currently gated by `object.editor.selectable` (serialized),
`ObjectConcept::selectable` / `editable` / `runtime_only`, and
`identity.type != GameObjectType::Runtime`, and `describe_object` re-forces flags after load.
Before object locking/protection lands, answer: is an operation forbidden because the *scene
concept* forbids it, or because *this authored object* was locked by the author? Keep those
two policies conceptually distinct; model per-object locks when the locking feature is built.
Do not add a large abstraction pre-emptively.

- [ ] Trigger: object locking / protection feature.

## C2. Reduce `WorldScene` by cohesive extraction

`WorldScene`'s size is not the problem; its accumulated responsibilities are. The next
cohesive subsystem is the command-application + authored-state cluster — and A2/A3/A4 should
largely dissolve or extract it already. After Phase A, review what is left
(`capture_authored_objects`, spawn positioning, `is_space_free`, creation actions, debug UI)
and extract only if a cohesive concern remains. Refactor by responsibility, not line count.

- [ ] Trigger: after Phase A, as a review checkpoint.

## C3. Make `GameObject` authored vs runtime field ownership explicit

`GameObject` is not a god object and its grouping is fine. Rotation fits in `transform`;
collision shape data fits in `collision`. The thing to watch is the mix of authored data
(`transform`, `size`, `render`, `collision`) and transient runtime data (`motion.velocity`,
`motion.facing`, `lifecycle.remaining`) in one struct. Before grouping / prefab / layer
relationships arrive, make that ownership explicit rather than adding every new concern
directly to `GameObject`. Do not pre-emptively redesign it, and do not reach for an ECS.

- [ ] Trigger: behaviour configuration, authored events, grouping, or prefabs.

---

# Sequencing summary

| Order | Item | Blocks / enables | Risk |
| --- | --- | --- | --- |
| P0.1 | `.clang-format` tree-wide | Clean refactor diffs | Low |
| P0.2 | Sanitizer build + tests in CI | Safety net for A2/A3 | Low |
| P0.3 | Second compiler / Linux build | Catches lifetime/template bugs per PR | Low–medium |
| P0.4 | Lifetime / pointer-validity ADR | Formalises the A3 rule | Low, write with A3 |
| A1 | Split authored config vs runtime state; trim the save file | Clean settings node for A2/A4 | Low |
| A2 | `EditorCommand` typed payloads | Rotation; A3/A4 read paths | Low, mechanical |
| A3 | Authored store authoritative | Removes reconciliation machinery | Medium — the core change |
| A4 | Scene-specific edits go through commands | Scene settings panel, new authored config | Low once A2/A3 done |
| A5 | Authoring-contract integration tests | Safe iteration on everything above | Low |
| B1 | Explicit object concept identity | Behaviour toggles, authored events | Low |
| B2 | Scene ImGui out of runtime classes | Playable export, headless tests, B5 | Medium |
| B3 | Editor/session state off runtime world state | — | Medium |
| B4 | Selection out of `GameObjectManager` | Grouping, hierarchy | Low, do with B3 |
| B5 | CMake runtime/editor split | Enforces B2 | Low, blocked by B2 |
| C1 | Concept policy vs authored locks | — | Resolve before locking |
| C2 | `WorldScene` extraction | — | Falls out of Phase A |
| C3 | `GameObject` authored/runtime split | — | Monitor |

Then resume feature work: Rotation → Background → Text → Behaviour toggles (after B1) →
Authored events → Layers → Grouping → Prefabs → Asset browser → Playable export (after B2/B5).

---

# Decisions

Resolved while planning this document:

1. **Edit mode (A3).** Add a **Stop** action = rebuild the live world from the authored
   baseline + hold paused. Stop is the editable state; authored fields are editable only when
   stopped (or before first Play). Pause keeps meaning "freeze exactly this frame" and does
   not enable editing. Toolbar shows Stop / Play / Pause per state.
2. **Camera state (A1).** All camera state leaves the save file — editor pose and game
   framing. Scenes set their game camera framing on both `new_scene` and load, from the scene
   factory / `restore_defaults`-style logic.
3. **Grid options (A1).** Editor session preference, not per-scene save data.
4. **Scene settings command payload (A2/A4).** `SceneSettingsChange` holds two `YAML::Node`
   snapshots of the authored settings section; apply/undo call a scene virtual that loads the
   snapshot, reusing the existing per-scene serializers.

# Open questions

None outstanding. Raise new ones here as the work surfaces them.
