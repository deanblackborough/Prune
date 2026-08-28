# Prune decisions

This file records decisions (ADR) that I have made for Prune. 

When creating something it is easy to get swept up in the latest and greatest and/or try to do it all. This document is to ground me and remind why I made certain choices early on.

The aim is to explain:

- what Prune is deliberately doing
- what Prune is deliberately not doing
- what has been considered and deferred
- what would need to change before revisiting a decision

Active decisions are grouped by area. Decisions that have been completed or replaced are retained at the bottom under "Resolved and superseded decisions" so their original rationale is not lost.

# Project lifecycle decisions

## Save compatibility decision

### Decision

Existing `.yml` scene files are allowed to break during this phase.

### Why

The object model, scene descriptors, behaviour ids, concept metadata, and scene-specific save data are still very much being designed and shaped, I don't know what I want the final save form to look like yet, it is not worth the effort to maintain compatibility with a save format that is still being designed.

Whilst I am still very obviously in alpha and soon 0.x.x territory I don't want to feel locked into a save format designed when one scene existed and all you could do was move a box around.

### Consequences

- Save files are not a public contract yet.
- Scene files can be regenerated when the model changes.

### Revisit when

- The scene-type model has settled.
- The core idea has been proven by multiple scenes.
- The core idea has been proven with multiple editor tools.
- The project has users who need save compatibility.
- I release a beta or stable version.

---

# Technology decisions

## C++23

### Decision

Use C++23.

### Why

Prune is partly a C++ learning and portfolio project. C++23 was the latest published standard when I started the project.

---

## SDL2

### Decision

Use SDL2 for windowing, input, and the basic runtime foundation.

### Why

SDL is mature, widely used, cross-platform, and low-level enough that Prune still owns its editor/runtime architecture.

It gives Prune a practical base without becoming the product itself.

### Why not SDL3 yet

SDL3 may be the future direction, but SDL2 is currently sufficient for what Prune is doing and has already proven itself in the project.

I will think about migrating to SDL3 when there is something which is not possible with SDL2.

### Why not Raylib

Raylib is excellent and I have been looking at it and playing with it for a while.

Prune is not a game, it is about building a live editor/runtime where scene types own their own tools, inspectors, object semantics, and workflow.

Raylib is deliberately higher-level and friendlier. That is a strength, but it would hide or reshape some of the editor/runtime boundary work that Prune is trying to explore.

Also, the current stack already works well with Dear ImGui and SDL2.

### Why not SFML

SFML is a reasonable 2D multimedia library, but SDL is the more obvious fit for a low-level, cross-platform editor/runtime foundation with mature ecosystem usage.

Prune does not need a friendlier 2D abstraction layer at this stage. It needs control over viewport, input, rendering integration, and editor/runtime boundaries.

---

## SDL2_image

### Decision

Use SDL2_image for loading basic image assets.

### Why

Prune needs simple sprite support without building an asset pipeline yet.

SDL2_image is enough for loading PNGs and proving rectangle/sprite rendering inside the editor/runtime.

### Why not build a custom asset pipeline now

A custom asset pipeline would be premature. The current goal is proving scene types, object semantics, tooling, and save/load boundaries.

### Revisit when

- Sprite/resource handling becomes painful.
- Asset references need validation, packaging, previews, or import rules.
- The editor needs a real resource browser.

---

## Dear ImGui

### Decision

Use Dear ImGui for the editor UI.

### Why

Dear ImGui is a strong fit for developer tools, debug interfaces, inspectors, overlays, and fast iteration.

Prune needs to evolve UI quickly while the architecture is still moving. Immediate-mode UI supports that.

### Why not Qt

Qt is powerful, but it would make Prune feel more like a desktop application framework project rather than a live game editor/runtime prototype.

### Why not a web UI

A web UI would add process, rendering, input, and integration complexity that distracts from the core idea.

Prune's value is in the live editor/runtime boundary, not in proving browser-based tooling.

### Why not write a custom UI toolkit

That would be a separate project.

Prune needs editor tooling, not a UI framework. I want to focus on the game tools and how the editor works rather than building a UI library when ImGui already exists and is widely used for game tooling.

### Revisit when

- ImGui goes away, so realistically never.
- I outgrow ImGui, so realistically never.

---

## yaml-cpp / YAML save files

### Decision

Use YAML through yaml-cpp for scene save/load.

### Why

YAML is readable, easy to inspect during development, and useful while the scene model is still evolving.

Human-readable files make save/load debugging easier.

### Why not binary

Binary saves would be premature at this stage and make debugging everything that much harder for no real gain.

### Why not JSON

JSON would work and is often the default choice in web development. For Prune, YAML is a better fit because the scene files remain approachable and easy to inspect.

### Revisit when

- Save files become a stable compatibility contract.
- The format needs versioning/migrations.
- Load performance or file size becomes a real issue.
- I start shipping Prune as a product and need a more opaque format for save files.

---

## CMake

### Decision

Use CMake.

### Why

CMake is the practical default for C++ projects, works with Visual Studio, supports vcpkg integration, and is familiar to C++ tooling ecosystems.

### Why not project files only

Visual Studio project files would make the project less portable and less credible as a C++ open-source project.

### Revisit when

- CMake no longer supports the required toolchains or dependency workflow effectively.

---

## vcpkg

### Decision

Use vcpkg for dependencies.

### Why

vcpkg is practical on Windows, works well with CMake, and keeps dependency setup repeatable.

### Why not vendoring everything

Vendoring dependencies would add repository noise and maintenance work.

### Why not manually installed libraries

Manual dependency setup is fragile and makes the project less approachable.

### Revisit when

- Dependency setup becomes a major contributor friction point.
- CI/build packaging requires a different approach.
- An expert contributor comes along and offers to set up and explain a different and better approach to dependencies.

---

# Architecture decisions

## Not using an ECS / EnTT right now

### Decision

Do not use an ECS yet. Do not add EnTT yet.

### Why

EnTT is a strong C++ ECS library, and I used it in an earlier version of Prune. However, Prune does not currently need an ECS.

The current problem is not high-volume entity processing. The current problem is editor/runtime semantics:

- What is this object?
- Is it authored or runtime-created?
- Is it selectable?
- Is it editable?
- Is it persistent?
- Which scene systems are allowed to manipulate it?
- How should the inspector describe it?
- Should it appear in normal editor tooling?

An ECS would not answer those questions by itself. It could even make the current design harder to understand by adding another abstraction before the editor model has settled.

### Why this is not anti-ECS

This is not a claim that ECS is bad.

It is a sequencing decision.

Prune needs explicit object semantics and editor behaviour before it needs a data-oriented entity architecture.

### Revisit when

- There are many runtime objects.
- Behaviour composition becomes difficult with the current model.
- Multiple scene types need shared systems operating over large object sets.
- The editor semantics are stable enough that an ECS would support them rather than obscure them.

---

## WorldScene as the shared scene base

### Decision

Keep `WorldScene` as the shared foundation for world-style scenes.

### Why

The second scene type proved that scenes need a common shell but still need scene-specific behaviour.

`WorldScene` is the current host for shared viewport, camera, object manager, grid, render/update flow, selection, and generic scene editing.

### Why not make every scene fully standalone

That would duplicate editor shell behaviour and make it harder to prove Prune's core idea, that multiple scene types can coexist without a conditional mess.

### Why not make the base class too powerful

If `WorldScene` absorbs too much scene-specific behaviour, it becomes the conditional mess Prune is trying to avoid.

### Revisit when

- A third scene exposes shared behaviour that genuinely belongs in the base.
- A non-world scene type appears and does not fit `WorldScene`.

---

## Scene types own what makes them different

### Decision

A scene type should only need to define what makes it different.

Scene-specific code should own:

- object roles
- behaviour rules
- default object layout
- runtime state/options
- scene-specific inspector sections
- scene-specific creation actions
- scene-specific save data

### Why

This is the central Prune idea.

The shared editor shell should provide enough infrastructure that new scene types do not reimplement generic editor mechanics.

### Consequences

- Some duplication is acceptable until two or more scenes prove the shared shape.
- Do not abstract something just because it appears twice.
- Do abstract something when the scene-specific difference is clear and the shared part is stable.

---

## Scene-owned player controllers

### Decision

Player/controller behaviour belongs to the scene that defines the player rules.

The shared scene layer should not contain a generic `PlayerController` unless multiple scenes genuinely converge on the same control model.

### Why

The current scenes have different control needs:

- Simple Shooter uses top-down movement and firing.
- Platformer uses gravity, jumping, and grounded state.
- Artillery uses turn-based aim, power, and firing.

A shared controller at this stage would either be misleading or would grow conditionals for unrelated game types.

### Consequences

- Scene-specific controllers live with the scene-specific behaviour and state.
- Shared input helpers can still be introduced later if duplication appears.
- Future scenes should not reach into a generic controller by default.

### Revisit when

- At least two scene types need the same input-to-motion translation.
- The shared behaviour can be named without hiding scene-specific rules.

---

## ObjectConcept rather than full plugin API

### Decision

Introduce a small shared object concept description type.

Example direction:

```cpp
struct ObjectConcept {
    std::string_view id;
    std::string_view label;
    std::string_view purpose;
    std::string_view collision_rule;
    bool runtime_only = false;
    bool selectable = true;
    bool editable = true;
};
```

### Why

The editor needs to understand what an object means in the active scene.

A small concept description gives the inspector, outliner, overlays, and future tools a shared language without turning Prune into a plugin framework.

### Why not a plugin API now

A plugin API would freeze too much too early. Prune is not ready to define that public extension boundary.

### Revisit when

- Third-party scene types become a realistic goal.
- The internal scene API stops changing rapidly.
- There is a real example of an external scene type being built outside the main repo.

---

## SceneDescriptor rather than dynamic plugin registration

### Decision

Use static scene descriptors before any dynamic registration/plugin system.

A descriptor can identify:

- scene type enum
- scene id
- label
- default file path
- description

### Why

The UI, factory, save/load, and docs need a shared way to talk about available scene types.

A static descriptor list is enough for now.

### Why not plugin registration

Dynamic registration would imply a public extension model that Prune does not have yet.

### Revisit when

- Scene types are stable.
- Adding scenes outside the core codebase becomes a priority.
- The project needs optional scene modules.

---

## Scene-specific serializers stay

### Decision

Keep scene-specific serializers.

### Why

Scene types own scene-specific data.

Generic serialization should cover shared object/state data. Scene-specific serializers should cover scene-specific options, defaults, and runtime-relevant authored data.

### Why not one generic serializer

A fully generic serializer would either miss scene meaning or become filled with scene-specific conditionals.

### Revisit when

- The common save/load wrapper is stable.
- Scene-specific save data follows a repeated enough shape to justify a helper.

---

# Tooling decisions

## Editor tooling before more complete games

### Decision

Prioritise everyday editor tooling before expanding the sample scenes into larger games.

### Why

Prune's value is the editor/runtime model, not the complexity of the sample games. The samples are proof slices; building a complete game with Prune is a much later goal.

More gameplay does not by itself prove Prune's core idea. Until the editor is ready, a separate game project is better served by an established engine such as Godot or Unreal.

### Consequences

- Prioritise workflows that make authored scenes easier to create, understand, and maintain.
- Expand sample gameplay when it demonstrates existing tooling or creates justified pressure for a shared editor feature.
- Do not treat sample-game content as a substitute for strengthening the editor/runtime boundary.

### Revisit when

The editor has enough tooling to make the slices feel buildable rather than merely inspectable.

---

## Debug overlays before visual polish

### Decision

Prioritise debug overlays before broad visual polish.

### Why

Debug overlays directly prove scene/object semantics:

- collision bounds
- runtime object markers
- role labels
- selected object outlines

They also make screenshots and demos easier to understand.

### Revisit when

The project needs presentation polish for a release/demo video.

---

## Undo/redo through editor commands

### Decision

Undo/redo should be built on an editor command/change model, not as one-off reversal logic inside individual tools.

### Why

Prune is becoming a live editor surface. Movement, creation, deletion, duplication, renaming, sprite changes, and inspector edits all need the same history rules.

If undo/redo is bolted into each feature separately, the project will accumulate inconsistent edge cases quickly.

### Consequences

- The first undo/redo implementation should be editor-only.
- Runtime/gameplay events should not enter editor history.
- Commands should be recorded when an edit is committed, not every frame of a drag.

### Revisit when

- Runtime recording, replay, or gameplay rewind becomes a deliberate feature.
- Multi-scene editing exists.
- Save/load history restoration becomes important.

---

## Keep editor command history unbounded for now

### Decision

Do not add a command-count or memory limit to editor undo/redo history yet. Treat a bounded history policy as known future work rather than clearing history during normal saves.

### Why

Editor commands retain before/after object snapshots so undo and redo can restore committed authored changes. This means command history can grow throughout a long editing session, especially when commands affect multiple objects.

Current scenes and editing sessions are small enough that there is no measured memory problem. Choosing an arbitrary limit now would add policy and UI behaviour without evidence for the right boundary. Clearing history on save would also make saving unexpectedly remove useful undo state.

The separate authored runtime-reset baseline is not an accumulating history: it keeps one current copy of each authored object. The unbounded growth risk belongs to editor command history.

### Consequences

- Saving a scene does not clear undo/redo history.
- Command history may consume increasing memory during a very long editing session.
- No command-count or memory-budget setting is exposed yet.
- A future limit should preserve predictable undo behaviour and should not be tied implicitly to Save.

### Revisit when

- Memory growth becomes measurable during realistic editing sessions.
- Scenes or command snapshots become substantially larger.
- Long-running editor sessions become a normal workflow.
- A clear command-count limit, memory budget, or history-compaction policy can be chosen from evidence.

---

## Scale before rotate

### Decision

Keep Scale as the current final transform tool and defer Rotate until Prune's rendering, collision, persistence, and editor rules can support it coherently.

### Why

Scale fits the current object model: authored objects already have width, height, bounds, and inspector fields.

Rotation cuts across rendering, picking, collision, bounds, serialization, inspector behaviour, and gizmo math. Prune is still largely rectangle/AABB based, so rotation should wait until those assumptions are explicit.

### Consequences

- Select, Move, and Scale are the current generic editor tool modes.
- Rotation remains planned but deliberately later.
- Future rotation work should start by documenting what rotates visually and what rotates physically.

### Revisit when

- The object model can represent rotation explicitly.
- Rendering, picking, and collision rules for rotated objects are agreed.
- Rotation can participate in inspector editing and undo/redo consistently.

---

## Defer grouped inspector edit actions

### Decision

Implement basic group support for move and delete, but defer grouped inspector/property edits.

### Why

Grouped editor commands are now supported for multi-selection move and delete, because those actions need to undo and redo atomically as a single user action.

Grouped inspector/property edits introduce additional UI and command-history complexity:

- Mixed values need clear inspector behaviour.
- Partial edits need explicit rules.
- Command history must describe grouped edits accurately.
- Undo/redo must restore all affected objects as one atomic command.

For now, the inspector continues to edit the active selected object only. Multi-object mutation is limited to viewport movement and deletion.

### Revisit when

The tools and command-history model have been stable through normal multi-selection use, and the expected behaviour for mixed values and partial edits is clear.

---

## Explicit editor tool mode state

### Decision

Use explicit generic editor tool modes rather than inferring editor behaviour from whichever viewport handle was clicked.

### Why

Selection, movement, and scaling have different interaction rules. Explicit modes make the active behaviour visible to the user and give viewport input, tooling UI, and command history one shared source of truth.

### Consequences

- Select, Move, and Scale are explicit modes.
- The active tool is transient editor state and is not saved into scene files.
- New tools need their own interaction rules and undo labels before being added.

### Revisit when

- A scene-specific tool needs to coexist with the generic modes.
- Tool shortcuts or temporary tool overrides are introduced.
- Rotate is ready to become a supported editor mode.

---

## Godot-style sparse z-index for authored render order

### Decision

Give authored objects a single `int z_index` render-order field, modelled on Godot:

- Default `0`. Most objects are never touched.
- Higher `z_index` renders later, so it draws on top.
- Equal `z_index` falls back to existing insertion/authoring order (`stable_sort`).
- No compaction, no normalisation, no gap management. The value is whatever the user set.
- It is a render concern only. It does not affect collision, picking, or bounds.
- Runtime-only objects ignore `z_index` entirely and always render above all authored objects, in spawn order.

Editing is a plain integer field plus Raise/Lower (`+1` / `-1`) buttons in the inspector Render section, recorded as a `ChangeObjectZIndex` editor command that participates in dirty state, undo/redo, and the authored baseline like any other single-object property edit.

### Why

The common need is small: usually just "the player draws in front of that platform". A sparse integer that stays at `0` for almost everything matches how people already think about Godot's `z_index`, and keeps the mental model tiny.

A dense compacted order-key (0..N-1 rewritten on every reorder) was rejected: it makes every reorder touch many objects, complicates undo, and buys nothing when the real use is nudging one or two objects.

A full layer system (named layers, separate render/collision layers) was rejected as out of scope for this phase. `z_index` is deliberately the smallest thing that makes render order authorable.

Keeping runtime objects always-on-top preserves the authored/runtime split (see "Runtime objects are not authored objects") and stays deterministic without extra bookkeeping during play.

### Consequences

- `RenderData` carries `z_index`; it serialises under `render.z_index` and is read as optional (missing means `0`), so older scene files load unchanged.
- Scene factories set `z_index` only where layering matters (player/tanks above terrain and platforms); everything else stays `0`.
- The scene renderer builds a per-frame sorted draw list. Object counts are small; a cached list is a later optimisation if it is ever needed.
- Multi-selection reorder and To-Front/To-Back are not implemented yet.

### Revisit when

- Scenes need grouped layers, or separate render and collision layers.
- Reordering many objects at once becomes a common workflow.
- Per-frame draw-list sorting shows up in profiling.
- Outliner drag-and-drop ordering is added.

---

# Runtime decisions

## Runtime objects are not authored objects

### Decision

Runtime-created objects should be treated differently from authored scene objects.

### Why

Bullets, generated enemies, temporary effects, and similar runtime objects should not become normal editor-authored content by accident.

This distinction is essential for save/load, undo/redo, selection, inspector behaviour, and future editor tools.

### Consequences

- Runtime objects should not be persisted by default.
- Runtime objects should not be renameable or cloneable by default.
- Runtime objects may be visible through debug overlays.
- Runtime objects may be selectable only if an explicit debug mode allows it.

---

## Audio remains a code baseline until effects UI is designed

### Status

Implemented baseline; further audio code and UI are deferred.

### Decision

Treat the existing audio implementation as the complete audio baseline for the current phase. Do not add further audio-specific code or authored audio UI until the broader effects and reaction system is being designed.

Use lightweight scene event ids to provide basic audio feedback without committing to a complete authored event/reaction system.

Scenes may emit ids such as `player_fired`, `player_jumped`, `enemy_destroyed`, `player_hit`, and `round_reset`. The app/runtime layer maps those ids to hard-coded sound resources and passes them to the audio system.

### Why

The current samples need clear runtime feedback:

- Simple Shooter needs firing and enemy-destruction feedback.
- Platformer needs jumping and player-hit feedback.
- Artillery needs firing, explosion, player-hit, and round-reset feedback.

The existing implementation establishes the required code boundary: scene behaviour emits events, the app/runtime layer chooses sound resources, and the audio system handles playback. That is sufficient as a baseline.

The broader event/reaction model is not designed yet. Eventually an event may trigger sound, animation, sprite changes, UI effects, screen shake, object spawning, or scene-specific behaviour. Audio authoring will need many of the same concepts as those other reactions, including event bindings, resource selection, configuration, and validation. Designing an audio-only UI now could establish the wrong abstraction and require it to be replaced when effects support is added.

### Consequences

- Scene behaviour emits event ids and does not call audio playback directly.
- Scene behaviour does not know which sound file is used, how it is loaded or mixed, or whether audio is enabled.
- Event-to-sound mappings remain hard-coded for now.
- Sound resources and the global audio toggle remain small and explicit.
- No further audio-specific code is required for the current phase.
- Event bindings are not authored or serialized yet.
- No audio authoring panel, resource picker, asset browser, or complete event-management UI is introduced by this decision.
- Audio UI will be considered as part of the broader effects/reaction authoring design rather than as an isolated feature.

The mapping is intentionally replaceable; the boundary between event producers and consumers is the part intended to remain.

### Rejected alternative

Direct sound calls inside scene behaviour were rejected because they would couple gameplay rules to audio playback and make broader event-driven reactions harder later.

### Revisit when

- Work begins on authored effects or reactions.
- One event needs to trigger multiple reaction types.
- Event bindings need to be authored or serialized.
- The editor has an asset model capable of selecting and validating reaction resources.
- Hard-coded mappings become difficult to maintain across the sample scenes.

---

# Scene slice decisions

## Keep sample scenes focused and recognisable

### Decision

Keep Simple Shooter, Platformer, and Artillery as focused proof slices rather than full games.

Once the editor naturally supports the required features, each sample may grow into a small, recognisable slice of a well-known game style. That growth should demonstrate Prune's capabilities rather than drive unrelated editor architecture prematurely.

### Why

The samples exist to prove that one editor/runtime model can support meaningfully different game types:

- Simple Shooter proves top-down movement, shooting, runtime projectiles, enemies, and authored collision.
- Platformer proves gravity, jumping, grounded movement, platforms, hazards, reset points, and different camera expectations.
- Artillery proves turn-based control, generated terrain, two controlled actors, projectile arcs, and a different scene-specific tool surface.

Recognisable, enjoyable slices make Prune easier to understand and demonstrate. Full-game scope such as extensive progression, content, menus, enemy varieties, or level systems would distract from the editor/runtime architecture.

Small additions that improve family play or demo value are welcome, but they should not become the core focus of Prune or force tooling that the editor does not otherwise need.

### Consequences

- Sample features should prove editor/runtime behaviour or materially improve demo value.
- New gameplay should use tooling that already exists or is justified independently.
- The samples may become richer, but they should remain bounded slices rather than complete games.

### Revisit when

- The relevant tooling exists naturally rather than being forced by a sample.
- A sample needs more depth to demonstrate a completed editor feature.
- There is dedicated time for sample polish rather than editor and runtime architecture.

---

## Do not add a fourth scene yet

### Decision

Do not add a fourth scene type until the three current scene types have exposed and resolved the next shared boundaries.

### Why

Simple Shooter, Platformer, and Artillery now prove enough variety:

- free movement and projectile combat
- platform collision and grounded movement
- turn-based artillery with generated authored terrain

The third scene has done its job. It exposed the next architectural pressure points:

- editor input vs runtime input ownership
- scene-specific load transaction safety
- scene-specific generated authored data
- clearer authored/runtime object rules
- stronger documentation around what belongs in `WorldScene`

Adding another scene now would mostly multiply unresolved design pressure.

### Consequences

- Improve the shared editor/runtime foundation before increasing scene count.
- Use the existing three scenes as regression coverage for architecture changes.
- Treat new scene ideas as design notes, not implementation targets.

### Revisit when

- Editor/runtime input ownership is explicit.
- Save/load is transaction-safe for scene-specific state.
- Generated authored objects are documented and handled consistently.
- Runtime-only objects are consistently excluded from normal editing and persistence.
- The current three scenes remain coherent after another tooling pass.
- The current three scenes have been iterated on enough to feel like they are showing off the potential of Prune rather than just being basic proof of concept slices.
- Tooling exists and is stable enough to make a new scene type feel unique rather than more of the same.

---

# Documentation decisions

## README should explain the direction, not every class

### Decision

Keep README focused on project intent, current state, architecture direction, screenshots, and build instructions.

### Why

The code is still moving too quickly for heavy API documentation.

### Revisit when

The architecture stabilises enough for a scene-type authoring guide.

---

## NOTES.md is the active development plan

### Decision

Use `NOTES.md` for the current phase plan. It records what I intend to work on over the next period.

### Why

It is the working map for what to build next.

### Consequences

- Notes can be blunt.
- Notes can contain temporary decisions.
- Notes can change as the code proves or disproves assumptions.

---

## DECISIONS.md records why, not how

### Decision

Use this file to record decision rationale.

### Why

README explains what Prune is.

NOTES explain what is next.

DECISIONS explain why certain paths are being taken or deliberately avoided.

---

# Resolved and superseded decisions

These decisions are retained for their historical rationale but no longer describe current work.

## Object semantics before more scene types

### Decision

Do not add a third scene until object semantics are stronger.

### Why

The first two scenes exposed that objects relied too heavily on string behaviours, flags, colour, and scene-local interpretation. Adding another scene at that point would have multiplied that weakness.

### Revisit when

- Objects clearly expose scene meaning.
- Runtime/authored object separation is visible.
- Inspector, outliner, and tooling can reuse scene concept metadata.
- Scene-specific creation is cleaner.

### Status

Resolved. Artillery was added as the third scene after the current object semantics and concept metadata were in place.

---

## Undo/redo is deferred, not ignored

### Decision

Do not build undo/redo until the edit model, authored/runtime distinction, creation actions, and object semantics are clear.

### Why

Undo/redo needed a clearer edit model before individual tools started accumulating incompatible reversal logic.

### First undo/redo target

The first implementation should cover authored object movement, renaming, resizing, creation, and deletion. It should not initially include runtime simulation, projectiles, enemy spawning, camera movement, save/load, or full scene reset.

### Status

Superseded by "Undo/redo through editor commands". Undo/redo is now implemented using editor command snapshots.

---

## Simple transform gizmo first

### Decision

Start viewport transform tooling with move handles. Rotation and scaling can wait until movement respects viewport focus, object editability, protected runtime objects, and undo/redo.

### Why

Movement is the most common edit operation and the simplest useful proof of viewport tooling.

### Status

Resolved. Move handles were introduced first; explicit Move and Scale modes and undo/redo integration were added afterwards.

---

## Third scene should be artillery/tank, not card game

### Decision

Use an artillery/tank slice as the third scene rather than a card game.

### Why

Artillery tests turn-based flow, generated terrain, two player-controlled actors, projectile arcs, collision with terrain and tanks, and a scene-specific panel that is not just movement tuning.

A card game remained personally interesting, but it would pull Prune toward UI layout, hands, decks, zones, drag/drop rules, and data modelling before the world-scene editor/runtime foundation was ready.

### Status

Resolved. Artillery was added as the third scene type. A card-game slice remains a possible future direction when non-world and UI-heavy scene types become a deliberate focus.
