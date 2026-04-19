
# Prune

Prune is a small C++23 2D engine/editor experiment built with SDL2, Dear ImGui, and YAML.

The goal is not a traditional “edit here, play there” workflow. The long-term aim is a live 2D play-and-build system where editing and gameplay coexist, and where different scene types can define their own tools and behaviour instead of being forced into one editor model.

<img width="1917" height="1125" alt="image" src="https://github.com/user-attachments/assets/9bdae32a-2827-4505-ab2c-c84557b33b25" />

## Current state

Prune currently includes:

- SDL2 window and renderer
- live ImGui editor UI
- object selection and inspection
- player movement and collision
- editor grid with optional snapping
- basic camera movement
- object creation, cloning, renaming, and deletion
- YAML scene save/load

It is intentionally still small. The focus is on building a clear tooling foundation first.

## Tech

- C++23
- SDL2
- Dear ImGui
- yaml-cpp
- CMake
- vcpkg
- MSVC / Visual Studio 2022+

## Current features

### Scene and object editing
- Select objects with the mouse
- Edit transform, size, colour, and flags
- Rename objects with unique naming
- Clone and delete non-player objects
- Search objects in the outliner
- Highlight the selected object

### Player and world interaction
- WASD player movement
- Collision detection against solid objects
- Live player speed editing

### Editor tools
- Grid overlay
- Optional snap-to-grid
- Keyboard nudging for selected objects
- Editor camera movement
- Stats, controls, and options panels

### Persistence
- Save the current scene to YAML
- Load a saved scene from YAML

The save currently includes:

- scene ids and selection
- camera position
- grid settings
- scene options
- object data and flags

## Controls

- **WASD**: move player
- **IJKL**: move editor camera
- **Arrow keys**: nudge selected non-player object
- **Shift + Arrow keys**: larger nudge
- **Left click**: select object

## Roadmap

Near term:

- finish world space and camera properly
- separate editor camera from player camera
- improve object and scene type foundations
- add static sprites
- add gizmos for movement and scaling

Longer term:

- multiple scene types
- richer save/load
- animated sprites
- undo/redo
- ECS integration with EnTT

## Build

### Requirements

- Visual Studio 2022 or later
- CMake
- Git
- vcpkg
