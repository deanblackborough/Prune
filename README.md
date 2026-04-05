# Prune

## Minimal C++23 2D engine foundation

Prune is a deliberately small, explicit application layer built on SDL2.

The goal is not to be feature-rich, but to be clean, understandable, and extensible.

---

## Current Status

This project currently provides:

- SDL window + renderer with configurable settings
- Main application loop (fixed timestep + variable render)
- Scene system with inspector and debug UI
- Frame timing (high-resolution, SDL performance counters)
- Input handling (keyboard + mouse, ImGui-aware)
- GameObject management with selection and editing
- Player movement with collision detection
- Dear ImGui integration (inspector panel, debug stats)
- Explicit ownership (no globals, no service locator)

If it feels “bare”, that’s intentional.

This is a foundation, not a framework.

<img width="1921" height="1128" alt="image" src="https://github.com/user-attachments/assets/05166c04-f845-489e-afcb-616cefee6353" />

---

## Tech Stack

- C++23
- SDL2 – windowing, input, and rendering
- Dear ImGui – editor UI and tooling
- CMake – build system

---

## Core Architecture

- **App**  
  Owns lifecycle, main loop, and high-level orchestration

- **Window**  
  Wraps SDL window and renderer

- **Time**  
  Handles frame timing and fixed timestep updates

- **Input**  
  Manages keyboard and mouse state with frame-based polling

- **Scene**  
  Abstract interface for game logic and rendering

- **SandboxScene**  
  Example implementation with player, objects, and collision

- **GameObjectManager**  
  Manages game objects with selection and ID-based lookup

- **EditorUI**  
  Dear ImGui-based inspector and debug panels

---

## What You Should See

Running the project should:

- Open a 1280x720 window
- Display a player (green square) and a static block (orange)
- Allow player movement using arrow keys or WASD
- Show an inspector panel on the right with object properties
- Support object creation, cloning, deletion, and editing
- Click objects to select them
- Toggle debug stats via View menu

---

## Build Instructions (Windows + CLion)

### Requirements

- Visual Studio 2022 (MSVC toolchain)
- CMake (CLion bundled is fine)
- Git
- vcpkg

---

### 1. Install vcpkg

```bash
cd C:\
mkdir dev
cd dev
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install sdl2:x64-windows
```

### 2. Configure CLion

Add the following CMake option:
```-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake```

### 3. Build and Run

- Open the project in CLion
- Let CMake configure 
- Build the Prune target 
- Run

## The plan
- Texture loading (SDL textures)
- Sprite rendering
- Camera system (2D)
- ECS (e.g. EnTT)
