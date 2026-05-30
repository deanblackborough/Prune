# Prune decisions

This file records decisions that I have made for Prune. When building something it is easy to get swept up in the latest and greatest, this document is to ground me and remind why I made certain choices early on.

The aim is to explain:

- what Prune is deliberately doing
- what Prune is deliberately not doing
- what has been considered and deferred
- what would need to change before revisiting a decision

## Save compatibility decision

### Decision

Existing `.yml` scene files are allowed to break during this phase.

### Why

The object model, scene descriptors, behaviour ids, concept metadata, and scene-specific save data are still very much being designed and shaped.

Whilst I am still very obviously in alpha and soon 0.x.x territory I don't want to feel locked into a save format designed when one scene existed and all you could do was move a box around.

### Consequences

- Save files are not a public contract yet.
- Scene files can be regenerated when the model changes.

### Revisit when

- The scene-type model has settled.
- The core idea has been proven by multiple scenes.
- The core idea has been proven with multiple editor tools.
- There is a following
- I release a beta or stable version.

---

# Technology decisions

## C++23

### Decision

Use C++23.

### Why

Prune is partly a C++ learning and portfolio project. C++23 is the latest standard when I started the project.

---

## SDL2

### Decision

Use SDL2 for windowing, input, and the basic runtime foundation.

### Why

SDL is mature, widely used, cross-platform, and low-level enough that Prune still owns its editor/runtime architecture.

It gives Prune a practical base without becoming the product itself.

### Why not SDL3 yet

SDL3 may be the future direction, but SDL2 is currently sufficient for what Prune will be doing and proven.

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

Qt is powerful, but it would make Prune feel more like a desktop application framework project  rather than a live game editor/runtime prototype.

### Why not a web UI

A web UI would add process, rendering, input, and integration complexity that distracts from the core idea.

Prune's value is in the live editor/runtime boundary, not in proving browser-based tooling.

### Why not write a custom UI toolkit

That would be a separate project.

Prune needs editor tooling, not a UI framework, I want to focus on the game tools and how the editor works, not building a UI library when ImGui already exists and is already used in so many tools in the gaming industry.

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

JSON would work, nearly always the choice in the Web world but this for Prune, YAML is a much better fit, all audiences can understand and parse YAML.

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

- Never

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

EnTT is a strong C++ ECS library, I used it in an earlier version of Prune, but, Prune does not need an ECS.

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

## Object semantics before more scene types

### Decision

Do not add a third scene until object semantics are stronger.

### Why

The current two scenes have already exposed the key architectural issue: objects rely too much on string behaviours, flags, colour, and scene-local interpretation.

Adding another scene now would multiply that weakness.

### Revisit when

- Objects clearly expose scene meaning.
- Runtime/authored object separation is visible.
- Inspector/outliner/tooling can reuse scene concept metadata.
- Scene-specific creation is cleaner.

### Status

Artillery was added as a third scene type after the current object semantics were already in place.

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

A plugin API would freeze too much too early, we are not anywhere near there.

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

## Undo/redo is deferred, not ignored

### Decision

Do not build undo/redo yet.

### Why

Undo/redo needs a clearer edit model.

The project first needs to define authored vs runtime objects, editable rules, creation actions, and object semantics.

### First undo/redo target

When added, start small:

- move authored object
- rename authored object
- resize authored object
- create authored object
- delete authored object

Do not initially include:

- runtime simulation
- bullets/projectiles
- enemy spawning
- camera movement
- save/load
- full scene reset

### Revisit when

- Transform gizmo exists.
- Scene-specific creation actions exist.
- Authored/runtime object distinction is enforced.

---

# Tooling decisions

## Editor tooling before more complete games

### Decision

After object semantics and duplication cleanup, focus on editor tooling before making the scene slices larger games.

### Why

Prune's value is the editor/runtime model, not the complexity of the sample games, they are just proof of concept slices, a real game with Prune is quite a way off from now.

More gameplay or better games do not prove the core idea of Prune is worthwhile, if I want to build the game I have planned I will use Godot or Unreal.

### First tooling targets

- selected object handles / transform gizmo
- collision/debug overlays
- scene role labels
- scene-specific creation actions
- better sprite picker

### Revisit when

The editor has enough tooling to make the slices feel buildable rather than merely inspectable.

---

## Simple transform gizmo first

### Decision

Start with move handles only.

### Why

Movement is the most common edit operation and the simplest useful proof of viewport tooling.

Rotation and scaling can wait.

### Consequences

- Must respect viewport focus.
- Must respect object editability.
- Must ignore protected runtime objects.
- Should integrate with undo/redo later.

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

# Scene slice decisions

## Keep Simple Shooter small

### Decision

Simple Shooter should stay a proof slice, not become a full game.

### Why

Its purpose is to prove:

- top-down movement
- facing direction
- shooting
- runtime bullets
- enemy movement
- collision cleanup
- scene-specific tuning
- scene-specific save data

Adding waves, scoring, powerups, menus, and extensive enemy types would distract from the editor/runtime architecture.

It does however need to be a bit of fun for my kids so I might add some of this stuff, it will just never be the core focus of Prune, my work with them in Godot will cover the majority of the gaming fix.

### Revisit when

The editor tooling is strong enough and the slice needs more demo value.

---

## Keep Platformer small

### Decision

Platformer should stay a proof slice, not become a full platform game.

### Why

Its purpose is to prove:

- gravity
- jumping
- ground checks
- platforms
- hazards
- player reset
- different camera expectations
- different object semantics from Simple Shooter

Adding scrolling levels, enemies, collectibles, slopes, ladders, and animation would be premature.

The same caveat applies as with Simple Shooter, I want to have fun with my kids and they love platformers, so I might add some of this stuff, but it will never be the core focus of Prune.

### Revisit when

The editor tooling needs more platformer-specific pressure.

---

## Third scene should be artillery/tank, not card game

### Decision

The recommended third scene is artillery/tank.

### Why

It tests a meaningfully different scene shape:

- turn-based flow
- generated terrain
- two player-controlled actors
- projectile arcs
- collision with terrain/tanks
- scene-specific panel that is not just movement tuning

### Why not card game yet

A card game is more interesting to me but it pulls the project towards UI layout, hands, decks, zones, drag/drop rules, and data modelling well before we have had a chance to prove the goal.

### Revisit when

- World-scene tooling is stronger.
- The editor/runtime model can handle non-world or UI-heavy scene types.
- Drag/drop and zone-based interactions become a deliberate focus.
- My Wife pushes for the card game slice, she loves card games and I have some ideas.

### Status

The artillery slice was added as the third scene type.

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

## Game slices as slices of well known games

### Decision

Initially keep the slices focused on proving the editor/runtime model. However, once we have the relevant tooling support, the slices should evolve to be slices of well-known games.

### Why

The point of the slices is to prove that Prune can handle many different game types and that it is not multiple editors in one. If the slices are recognisable slices of games people know and they feel and play well, it will be much easier for people to understand the point of Prune and more importantly, how capable it is.

### Why not yet

The current slices are enough to prove the concept, they are there to prove everything works and help me build the editor. Trying to make the game slices more complex now would distract from the core goal of Prune, they will be the icing on the cake.

### Revisit when

- The tooling naturally exists (not forced by the slices) to start fleshing out each of the slices.
- I have time to focus on the slices rather than the editor and engine architecture.

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

## Notes.md is the active development plan

### Decision

Use `Notes.md` for the current phase plan, it is what I plan to be working on over the next period.

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
