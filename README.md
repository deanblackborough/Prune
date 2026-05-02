# Prune

Prune is a C++23 2D editor-first game development experiment built with SDL2 and Dear ImGui.

It is not aiming to be a traditional engine where you “edit here, play there”.
Instead, the goal is a live system where editing and gameplay happen in the same space, with scene types defining their own behaviour, tools, and workflows.

## Why this exists

My earlier attempt [`prune-2d`](https://github.com/deanblackborough/prune-2d) leaned too far 
towards “build engine pieces first and hope the game and editor appeared later”.

Prune is take two.

The goal here is to build the runtime, editor, and interaction model together from the start 
so the project grows around live editing rather than adding tooling later. That means solving editor problems early: scene ownership, input focus, picking, inspection, save/load, and the separation between editor and game behaviour.

## Screenshot

Current editor state:

<img width="1921" height="1130" alt="image" src="https://github.com/user-attachments/assets/768f6685-5962-4b6b-ae88-564f5bf0263d" />

The scene viewport is interactive — objects can be selected, dragged, and edited directly.

Everything shown is live; there is no separate “play mode”.

## Philosophy

Most tools separate the editor and the game.

Prune is exploring a different direction:

- The editor is part of the runtime
- The scene is always live
- Tools are defined by the scene, not globally
- Interaction happens in the viewport, not only through panels

This means a platformer scene, a top-down scene, or even a card game scene can define their own:
- input behaviour
- inspectors
- editing tools
- rendering rules

Rather than forcing everything into one editor model.

## Current State

Prune is early, but already behaves like a small editor rather than a demo.

### Editor

- Dedicated scene viewport (not full-window rendering)
- Outliner and inspector panels
- Live property editing
- Camera controls (pan, player follow)
- Grid rendering with optional snapping

### Scene Interaction

- Click to select objects
- Drag objects directly in the viewport (world-space correct)
- Keyboard nudging (grid-aware)
- Editor camera and player camera separation
- Player object with separate controls (WASD)

### Rendering

- Basic render types:
  - Rectangle
  - Sprite (resource-based, minimal implementation)
- Visibility and simple flags

### Serialization

- YAML-based save/load
- Scene state includes:
	- objects
	- transforms
	- grid settings
	- camera state

This is intentionally simple — clarity before abstraction.

## Roadmap

Short-term focus:

- Viewport improvements (zoom, focus, boundaries)
- Gizmos (move / rotate / scale)
- Improved sprite handling
- First scene-specific gameplay implementation

More detail: see [`notes`](Notes.md)

### Long-term direction

- Scene-defined tools and inspectors
- Play-and-edit simultaneously without mode switching
- Extensible architecture for different game styles

## Build

### Requirements

- C++23 compiler
- CMake
- vcpkg

### Dependencies:

- SDL2
- Dear ImGui
- yaml-cpp

Example:

```bash
cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake

cmake --build build
```

## Credits

- Icons by Kenney — https://www.kenney.nl

## Status

Active and evolving.

The focus is on building a usable editor alongside the engine, not stabilising APIs early.
Expect refactors as the architecture becomes clearer.
