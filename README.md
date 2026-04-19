# Prune

## Minimal C++23 2D engine foundation

Prune is a deliberately small game engine/editor, built with C++23 and SDL2.

The goal is a fun little engine my children and I can play with; everything is live all the time

---

## Current Status

This project currently provides:

- SDL2 window and renderer with configurable settings
- Scene system with ImGui-based editor UI
- Input handling (keyboard and mouse, ImGui-aware)
- GameObject system with manager (ENTT planned for future)
- Live inspector with property editing
- Player movement with collision detection
- Grid system with snapping and visual overlay
- Camera system with editor controls
- Click-to-select object picking
- Object cloning and deletion
- Custom ImGui abstraction layer for consistent UI

If it feels "bare", that's intentional;

<img width="1917" height="1125" alt="image" src="https://github.com/user-attachments/assets/9bdae32a-2827-4505-ab2c-c84557b33b25" />

---

## Tech

- **C++23** – Modern C++ with modules-ready structure
- **SDL2** – Windowing, input, and rendering
- **Dear ImGui** – Editor UI and tooling
- **Yaml-cpp** – A YAML parser
- **CMake** – Build system with vcpkg integration
- **MSVC** – Primary compiler (Visual Studio 2022+)

---

## Features

### Player Movement
- **WASD** controls for smooth movement
- Configurable movement speed (adjustable in Inspector)
- Collision detection and resolution against solid objects
- Velocity display in debug panel

### Editor Camera System
- **IJKL** to pan the camera
- **Shift + Arrow Keys** for faster camera movement
- Configurable camera speed
- Live camera position editing in Options panel
- Independent from player movement

### Editor Grid System
- Optional visual grid overlay
- Configurable grid size (32-128 pixels)
- Toggle snap-to-grid for precise object placement
- Nudge step configuration (8-32 pixels)
- Grid settings persist during editing session

### Object Management
- **Left Click** to select objects in the scene
- **Arrow Keys** to nudge selected object (respects nudge step)
- **Shift + Arrow Keys** for multi-step nudging
- Create new blocks with randomized colors
- Clone existing objects (maintains properties)
- Delete non-player objects
- Rename objects with automatic unique naming
- Search objects by name in Outliner
- Visual selection highlighting (toggleable)

### Inspector Panel
- Real-time property editing:
  - Transform (X, Y position with sliders)
  - Size (Width, Height)
  - Rendering (RGB color picker)
  - Player-specific speed control
- Object flags (Active, Visible, Solid)
- Computed properties (screen position)
- Player flag (read-only for player object)
- Clone and Delete actions with styled buttons

### Outliner Panel
- Hierarchical object list
- Real-time search/filter
- Click to select objects
- Visual indication of selected object
- Object count display

### Options Panel
- Scene options (selection highlighting)
- Grid configuration (show, snap, size, nudge step)
- Camera settings (X, Y, speed)
- Live updates to scene behavior

### Stats/Debug Panel
- Frame rate and delta time
- Object count and selection state
- Player position and velocity
- Grid and snap settings
- Performance metrics

### Custom UI System
- `property_table` – Two-column label/value layouts
- `layout` – Structural helpers (headers, separators)
- Minimal ImGui exposure in game code
- Reusable, styled UI components

## Scene Persistence

Prune supports basic scene save/load using YAML.

### Save / Load

Scenes are saved to a local file: `sandbox_scene.yml`

Use the **File** menu:

- **New Scene** – resets the scene to defaults
- **Save Scene** – writes the current scene to disk
- **Load Scene** – replaces the current scene with the saved file

Load is immediate and replaces all current state.

### What is saved

Scene state:

- next object id
- player id
- selected object id (if any)
- camera position
- grid settings
- scene options

Per object:

- id
- name
- transform (x, y)
- rectangle (width, height)
- colour
- active / visible / solid flags
- is_player flag

### What is not saved (v1)

- velocity
- editor UI state (panel visibility, layout, etc.)
- assets or external resources
- multiple scene types

### Notes

- The save format is human-readable YAML.
- Loading requires a valid file; invalid data will fail to load and leave the current scene unchanged.
- Exactly one player object must exist in a valid scene file.

---

## Next
### Foundation
- Camera and limitless world space
- Editor camera vs player camera separation
- Object types (player, blocks, platforms, hazards, etc.)
- Static sprites

### Editing tools
- Gizmos for object movement
- Gizmos for scaling the selected object

### Core systems
- Undo/redo system
- Improved save and load (v2)

### Content
- Animated sprites

### Architecture
- Multi-scene support (scene types: platformer, top-down, etc.)
- ENTT (ECS integration)

## Build Instructions (Visual Studio + CLion)

### Requirements

- **Visual Studio 2022** or later (MSVC toolchain with C++23 support)
- **CMake 3.20+** (bundled with VS or standalone)
- **Git**
- **vcpkg** (C++ package manager)

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
vcpkg install yaml-cpp:x64-windows
```

### 2. Configure CLion

Add the following CMake option:
```-DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake```

### 3. Build and Run

- Open the project in CLion
- Let CMake configure 
- Build the Prune target 
- Run

