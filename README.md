# Prune

## Minimal C++23 2D engine foundation.

Prune is a deliberately small, explicit application layer built on SDL2, OpenGL, and Dear ImGui.
The goal is not to be feature-rich, but to be clean, understandable, and extensible.

## Current Status

This project currently provides:

SDL window + OpenGL context
Main application loop
Frame timing (variable + fixed step scaffold)
Input layer (keyboard + mouse, per-frame state)
Dear ImGui integration (tooling only)
Explicit ownership (no globals, no service locator)

If it feels “bare”, that’s intentional.

This is a foundation, not a framework.

- C++23
- SDL2 – windowing + input
- OpenGL – rendering backend (basic init only)
- Dear ImGui – tooling/debug UI
- CMake – build system

## Core
- App → owns lifecycle, main loop, systems
- Window → SDL window + GL context
- Time → frame timing + fixed timestep
- Input → per-frame input state (no SDL leakage outside)

## Build Instructions (Windows + CLion)
### Requirements
- Visual Studio 2022 (MSVC toolchain)
- CMake (CLion bundled is fine)
- Git
- vcpkg

### 1. Install vcpkg
```
cd C:\
mkdir dev
cd dev
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install sdl2:x64-windows
```

### 2. Clone imgui

```
cd <your Prune project>
mkdir external
cd external
git clone https://github.com/ocornut/imgui.git
```

### 3. Configure CLion

```-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake```