# Prune

## Minimal C++23 2D engine foundation

Prune is a deliberately small game engine/editor, built with C++23 and SDL2.

The goal is a fun little engine my children and I can play with; everything is live all the time

---

## Current Status

This project currently provides:

- SDL window and renderer with configurable settings
- Scene system with imgui inspector and debug windows
- Input handling (keyboard and mouse are ImGui-aware)
- Game object and manager, ENTT on the way in the future
- GameObject management with selection and editing
- Player movement with optional collision detection
- Grid, snapping, and live settings
- Click to select any object in the scene

If it feels “bare”, that’s intentional; I'm adding the features we want to use. 

<img width="1919" height="1131" alt="image" src="https://github.com/user-attachments/assets/216bad60-281a-47ca-891c-e217d8962e08" />

---

## Tech Stack

- C++23
- SDL2 – windowing, input, and rendering
- Dear ImGui – editor UI and tooling
- CMake – build system

---

## Features

### Player Movement
- *WASD* controls for smooth movement
- Configurable movement speed
- Collision detection and resolution against solid objects

### Editor Grid System
- Optional visual grid overlay
- Configurable grid size (16-128 pixels)
- Toggle snap-to-grid for precise non-player object placement
- Nudge step configuration for fine control

### Object Management
- *Arrow Keys* move the selected object
- *Shift + Arrows* move the selected object with multiplier
- Click objects to select them in the scene
- Create new blocks with randomised colours
- Clone existing objects 
- Delete non-player objects
- Rename objects 
- Search objects by name 
- Visual selection highlighting

### Inspector Panel
- Real-time property editing (position, size, colour)
- Player speed adjustment
- Object flags (active, visible, solid)
- Object list with search filtering

### Debug Panel
- Frame rate and timing information
- Object count and selection state
- Player position and velocity
- Grid and snap settings

---

## Next
- Sprites, static and animated
- Gizmos for object placement and scaling
- Save and load
- Camera system, you should be able to move around a limitless scene
- ENTT at some point

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

