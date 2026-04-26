# Prune

Prune is a C++ 2D editor/runtime experiment built around a live play-and-build workflow.

The aim is not a traditional engine with a separate editor bolted on afterwards. 
The editor is part of the experience: scenes are live, editing and gameplay coexist, and 
different scene types should eventually be able to define their own tools, inspectors, and 
behaviour instead of being forced into one fixed editor model.

This project is intentionally focused on architecture and tooling foundations first: 
viewport ownership, input routing, scene/world separation, object editing, and persistence.

## Why this exists

My earlier attempt [`prune-2d`](https://github.com/deanblackborough/prune-2d) leaned too far 
towards “build engine pieces first and hope the game and editor appeared later”.

Prune is take two.

The goal here is to build the runtime, editor, and interaction model together from the start 
so the project grows around live editing rather adding tooling later. That means solving 
editor problems early: scene view ownership, input focus, picking, object inspection, 
save/load, and the separation between editor behaviour and game behaviour.

<img width="1917" height="1128" alt="image" src="https://github.com/user-attachments/assets/7e4eecc3-3acc-45ab-8f48-e118c09e5fd8" />

## Design principles

Prune is being created based on a few simple rules:

- **Editor-first, not editor-later**  
  The scene should live inside the tooling UI, not the other way round.

- **Live state over mode switching**  
  The long-term goal is a system where editing and gameplay coexist rather than a strict 
  “edit mode” vs “play mode” split, no play buttons and no separate editor process.

- **Explicit over magical**  
  Ownership, input routing, camera behaviour, and scene interaction should stay understandable.

- **Small steps over premature architecture**  
  New systems are added when I need them, not because “engines usually have one”, I made that mistake before.

- **Scene-specific tooling later**  
  Different scene types will eventually be able to define different tools and inspectors instead of 
  sharing a one-size-fits-all editor, the goal is a scene type for top down games, another for platformers with gravity, 
  another for card games, etc.

## Current state

Prune is still small on purpose.

The project is currently focused on getting the editor/runtime foundation right before 
moving into richer content or more advanced rendering. The main concern right now is not 
“how many features can be added?”, but whether the core structure is correct enough to 
support scene viewports, gizmos, scene-specific tools, and live editing without 
fighting the architecture later.

## Implemented so far

### Scene editing
- object selection
- object inspection
- object renaming with uniqueness handling
- object cloning and deletion
- outliner search
- selected object highlighting

### Runtime interaction
- player movement
- collision against solid objects
- live player speed editing

### Editor support
- editor grid
- optional snap-to-grid
- keyboard nudging
- editor camera movement
- stats and options panels

### Persistence
- YAML save/load for scene state, object data, grid settings, and camera data

## Controls

- **WASD**: move player
- **IJKL**: move editor camera
- **Arrow keys**: nudge selected non-player object
- **Shift + Arrow keys**: larger nudge
- **Left click**: select object

## Roadmap

### Next
- proper scene viewport inside the editor UI
- viewport-owned input and picking
- cleaner object/render type foundations
- static sprite support
- movement/transform gizmos

### After that
- scene-specific inspectors and tools
- richer persistence
- animated sprites
- undo/redo
- ECS integration where it genuinely helps rather than by default

### Long-term direction
- multiple scene types with different editing models
- a live play-and-build workflow where editor and runtime remain part of the same experience

## Tech

- C++23
- SDL2
- Dear ImGui
- yaml-cpp
- CMake
- vcpkg
- MSVC / Visual Studio 2022+

## Build

### Requirements

- Visual Studio 2022 or later
- CMake
- Git
- vcpkg

## Credits

- Icons by Kenney @ www.kenney.nl