# Prune

Prune is a C++23 live 2D editor/runtime experiment built with SDL2 and Dear ImGui.

It is not trying to be a traditional engine where editing happens in one mode and gameplay happens in another. The goal is a play-and-build system where the editor is part of the runtime, and different scene types can define their own behaviour, tools, panels, inspectors, and workflows.

## Why this exists

My earlier attempt, prune-2d, leaned too far towards building generic engine pieces first and hoping the editor and game would appear later.

Prune is take two.

The goal is to build the editor, runtime, and interaction model together from the start. That means solving editor problems early: viewport ownership, input focus, picking, object inspection, save/load, scene-specific behaviour, and the boundary between generic editor tools and scene-owned tools.

## Core idea

Prune is built around this split:

- generic editor shell
- generic object editing
- generic rendering and scene state
- scene-specific behaviour
- scene-specific tools and panels

A top-down shooter, platformer, card scene, or puzzle scene should be able to reuse most of the editor while still defining the parts that make that scene type unique.

## Current state

Prune currently has:

- Dedicated ImGui scene viewport
- Live object selection and dragging
- Outliner and inspector panels
- Grid rendering and snapping
- Editor camera and game camera separation
- YAML scene save/load
- Rectangle and sprite rendering
- A small simple-shooter behaviour slice:
  - player movement
  - facing direction
  - shooting
  - bullets
  - enemy movement
  - basic bullet/enemy collision
  - scene-specific tuning panel

This is still early, but it is now more than a rendering demo. The current focus is proving that live editing and scene-specific behaviour can coexist cleanly.

## Roadmap

Near-term:

- Rename the first scene type away from SandboxScene
- Tighten save/load for scene-specific settings
- Add runtime pause
- Improve sprite handling
- Add transform gizmos
- Extract the first real scene-type boundary
- Add a second scene type to test the architecture

Medium-term:

- Top-down shooter scene
- Platformer scene
- Card or grid-based puzzle scene
- Scene-specific inspectors
- Scene-specific tools
- Better sprite/facing/animation support
- Undo/redo
- Object duplication and multi-select
- Debug overlays
- Basic audio hooks

Long-term:

- Live play-and-build workflow
- Scene-defined tools
- Scene-defined runtime behaviour
- Multiple scene types sharing the same editor shell
- Fast prototyping workflow for game jams, education, and small 2D experiments

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