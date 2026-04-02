# Prune

## Minimal C++23 2D engine foundation

Prune is a deliberately small, explicit application layer built on SDL2.

The goal is not to be feature-rich, but to be clean, understandable, and extensible.

---

## Current Status

This project currently provides:

- SDL window + renderer
- Main application loop (fixed timestep + variable render)
- Scene system (basic abstraction)
- Frame timing (high-resolution, SDL performance counters)
- Input handling via SDL events
- Explicit ownership (no globals, no service locator)

If it feels “bare”, that’s intentional.

This is a foundation, not a framework.

---

## Tech Stack

- C++23
- SDL2 – windowing, input, and rendering
- CMake – build system

---

## Core Architecture

- **App**  
  Owns lifecycle, main loop, and high-level orchestration

- **Window**  
  Wraps SDL window and renderer

- **Time**  
  Handles frame timing and fixed timestep updates

- **Scene**  
  Abstract interface for game logic

- **SandboxScene**  
  Example implementation (movable square)

---

## What You Should See

Running the project should:

- Open a window
- Display a square
- Allow movement using:
    - Arrow keys or WASD

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
- Introduce Dear ImGui (SDL renderer backend)
