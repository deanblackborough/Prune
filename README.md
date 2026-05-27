# Prune

![Prune](assets/icon.png)

Prune is a C++23 live 2D editor/runtime prototype built with SDL2, SDL2_image, Dear ImGui, and yaml-cpp.

It is not trying to be a traditional engine where editing happens in one mode and gameplay happens in another. The goal is a play-and-build system where the editor remains part of the runtime.

Different scene types can share the same editor shell while defining their own behaviour, tools, panels, inspectors, object semantics, defaults, and save data.

> **Save compatibility warning**
>
> Prune is still pre-release. Existing `.yml` scene files may break while the object model, scene descriptors, behaviour ids, concept metadata, and scene-specific save data are being shaped.
>
> Save compatibility will matter later. For now, the priority is getting the scene model and editor/runtime architecture right.

## Why this exists

My earlier attempt, [prune-2d](https://github.com/deanblackborough/prune-2d), leaned too far towards building generic engine pieces first and hoping the editor and game would appear later.

Prune is take two.

The goal is to build the editor, runtime, and interaction model together from the start. That means solving editor problems early: viewport ownership, input focus, picking, object inspection, save/load, scene-specific behaviour, scene-specific tooling, and the boundary between generic editor tools and scene-owned tools.

## Core idea

Prune is built around a shared editor shell with scene-specific behaviour layered on top.

The shared side owns:

- Viewport
- Grid
- Camera foundations
- Selection
- Outliner
- Generic inspector
- Basic object editing
- Rendering foundations
- Save/load foundations
- Scene creation/loading flow

Each scene type owns:

- Object roles and semantics
- Runtime behaviour
- Scene-specific tools
- Scene-specific panels
- Scene-specific inspector sections
- Scene-specific object creation rules
- Scene-specific default layout
- Scene-specific save data

A simple shooter, platformer, artillery/tank game, card scene, or puzzle scene should be able to reuse most of the editor and general tools whilst still defining the parts that make that particular scene type unique.

The current goal is not to build complete games. The goal is to prove that multiple small game slices can coexist inside the same live editor/runtime without turning the scene layer into one large conditional mess.

As the projects develops, the game slices will improve and expand. Tools will be added when they are needed by the scenes, as these new tools get added the game slices, features and UX will improve.

### Platformer editor and runtime

![Platformer](assets/repo/platformer.png)

### Simple shooter editor and runtime

![Simple Shooter](assets/repo/simple-shooter.png)

## Prune is not...

Prune is not currently trying to be:

- A general-purpose commercial game engine
- A plugin-based editor framework
- An ECS experiment
- A complete platformer engine
- A complete shooter engine
- A stable scene file format

The project is still in the phase where concrete slices are being used to discover the right editor/runtime boundaries.

I have a [DECISIONS.md](DECISIONS.md) file which explains some of the choices I have made and why and more importantly why I have rejected to do some things.

## Current architecture direction

The project is moving towards this rule:

> A new scene type should only need to define what makes it different.

That means a scene type should not need to reimplement generic viewport access, camera access, object manager access, grid access, generic rendering flow, generic editor interaction, or basic save/load plumbing.

The current architecture is intentionally not a plugin system, not an ECS, and not a general-purpose engine API. The code is still being shaped around concrete scene slices first.

The next important step is stronger object semantics: the editor needs to understand what an object means in the active scene, not just that it has a rectangle, colour, transform, and runtime behaviour string.

## Where we are now

Prune has moved beyond a single-scene prototype. The project now has a shared editor/runtime foundation with two scene slices proving that different game types can reuse the same editor shell while owning their own behaviour, semantics, tuning, inspector sections, and save data.

The current focus is no longer “can a scene run?” It is now “can scenes be edited clearly and safely while the runtime remains live?”

Prune currently has:

- Dedicated ImGui scene viewport
- Shared editor camera and game camera foundations
- Grid rendering and snapping
- Live object selection
- Selected-object outline and first transform handle
- Handle-based object movement for authored movable objects
- Runtime object protection by default
- Outliner and generic inspector panels
- Scene-specific inspector sections
- Scene-aware object concepts (scene roles) for selection, editability, movement, runtime-only objects, and collision meaning
- YAML scene save/load
- Scene factory creation and scene-type loading from save files
- Rectangle and sprite rendering
- Basic sprite resource map
- Shared scene renderer, interaction, camera, state, collision, and serialization pieces
- Shared `WorldScene` foundation for scene types
- Simple Shooter scene slice
- Platformer scene slice

The first real editor tooling pass has started with the transform gizmo. It is intentionally small: selected authored objects show a visible manipulation affordance, and movement starts from the handle rather than from arbitrary object-body dragging.

This matters because it shifts Prune from “objects can be edited through panels” towards “the viewport itself is becoming an editor surface”.

## Editor model

Prune separates shared object editing from scene-specific behaviour.

The generic inspector shows shared object data and scene meaning; the scene-specific inspectors show what that object does in the current scene type.

- Scene meaning: What this object is in the context of the scene, authored or created by the runtime, selectable, editable, the purpose and collision rules.
- Scene type section: What the object is doing in the scene; for example, if it is the player, are they ready to shoot, is there a cooldown, what is the current speed, and so on.

## Current scene slices

The current scenes are proof slices. They are deliberately small, but they exercise different parts of the editor/runtime boundary.

They are not intended to be complete games yet.

### Simple Shooter

The Simple Shooter slice currently proves:

- Player movement
- Facing direction
- Shooting
- Runtime bullet objects
- Enemy movement
- Bullet/enemy collision
- Runtime cleanup
- Scene-specific tuning panel
- Scene-specific save/load data

### Platformer

The Platformer slice currently proves:

- Gravity
- Jumping
- Ground checks
- Solid platforms
- Hazards
- Player reset
- Scene-specific tuning panel
- Scene-specific save/load data

## Near-term focus

The scene-type model is now strong enough to begin the first real editor tooling pass.

The immediate focus is proving that viewport tools can operate safely on scene objects without bypassing scene ownership rules.

Current priorities:

1. Transform gizmo and selected-object handles (Done)
2. Collision/debug overlays
3. Scene-specific creation actions
4. Better sprite picker
5. Behaviour and save/load review
6. Third scene proof candidate

The key rule for this phase is that tools should ask the active scene what an object means before acting on it. A selected object is not just a rectangle; it may be authored, runtime-created, selectable, editable, movable, persistent, solid, hazardous, or scene-specific in some other way.

The current plan is tracked in [NOTES.md](NOTES.md).

## Documentation stance

It is too early for heavy internal documentation.

Useful documentation now:

- README explaining the project goal and current architecture direction
- Notes tracking next architecture work
- Short comments where C++ intent is not obvious
- Scene-type checklist once object semantics and scene descriptors settle

Too early:

- Full API docs
- Deep class documentation
- Plugin author guide
- Long tutorials

The code is still moving too quickly. Documentation should help decision-making without fossilising unfinished architecture.

## Build

### Requirements

- C++23 compiler
- CMake
- vcpkg

### Dependencies

- SDL2
- SDL2_image
- Dear ImGui
- yaml-cpp

Example:

```bash
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake

cmake --build build
```

## Acknowledgements

- [SDL2](https://www.libsdl.org/) — windowing, input, and rendering foundation
- [Dear ImGui](https://github.com/ocornut/imgui) — editor UI
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) — scene save/load
- [SDL2_image](https://github.com/libsdl-org/SDL_image) — sprite loading

## Licence

MIT — see [LICENSE](LICENSE)