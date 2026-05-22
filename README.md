# Prune

![Prune](assets/icon.png)

Prune is a C++23 live 2D editor/runtime experiment built with SDL2, SDL2_image, Dear ImGui, and yaml-cpp.

It is not trying to be a traditional engine where editing happens in one mode and gameplay happens in another. The goal is a play-and-build system where the editor remains part of the runtime, and different scene types can define their own behaviour, tools, panels, inspectors, and workflows without replacing the shared editor shell.

## Why this exists

My earlier attempt, [prune-2d](https://github.com/deanblackborough/prune-2d), leaned too far towards building generic engine pieces first and hoping the editor and game would appear later.

Prune is take two.

The goal is to build the editor, runtime, and interaction model together from the start. That means solving editor problems early: viewport ownership, input focus, picking, object inspection, save/load, scene-specific behaviour, scene-specific tooling, and the boundary between generic editor tools and scene-owned tools.

## Core idea

Prune is built around this split:

- Shared editor shell
- Shared object editing
- Shared viewport, grid, camera, outliner, inspector, stats, controls, rendering, and save/load foundations
- Scene-specific runtime behaviour
- Scene-specific tools and panels
- Scene-specific object creation rules
- Scene-specific inspector sections
- Scene-specific serialization data

A simple shooter, platformer, artillery/tank game, card scene, or puzzle scene should be able to reuse most of the editor while still defining the parts that make that scene type unique.

The current goal is not to build a full game. The goal is to prove that multiple small game slices can coexist inside the same editor/runtime without turning the scene layer into one large conditional mess.

### Platformer editor and runtime

![Platformer](assets/repo/platformer.png)

### Simple shooter editor and runtime

![Simple Shooter](assets/repo/simple-shooter.png)

## Current state

Prune currently has:

- Dedicated ImGui scene viewport
- Live object selection and dragging
- Outliner and inspector panels
- Inspector includes a scene-specific section which can be extended by each scene type
- Grid rendering and snapping
- Editor camera and game camera separation
- YAML scene save/load
- Scene factory creation and scene-type loading from save files
- Rectangle and sprite rendering
- Basic sprite resource map
- Shared scene renderer, interaction, camera, state, collision, and serialization pieces
- Simple Shooter slice
- Platformer slice

## Current scene slices

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

This slice is useful because it has runtime objects, spawned bullets, a persistent enemy, and top-down movement.

*It is still deliberately small. Before adding more tooling, this slice needs to be tightened so it feels like a clear demo rather than only an architecture test.*

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

This slice is useful because it has different movement rules, different object semantics, different camera expectations, and different tuning values from the shooter.

*It is still deliberately small. Before adding more tooling, this slice needs to be tightened so it feels like a clear demo rather than only an architecture test.*

## Near-term focus

I have a [notes](Notes.md) file which details everything I will be doing to clean up the core concept and prove Prune works and is worth building on.

## Documentation stance

It is too early for heavy internal documentation.

Useful documentation now:

- README explaining the project goal and current architecture direction
- Notes tracking next architecture work
- Short comments where C++ intent is not obvious
- Scene-type checklist once the shared shell exists

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
