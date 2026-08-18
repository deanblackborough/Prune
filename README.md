[![Build](https://github.com/deanblackborough/Prune/actions/workflows/build.yml/badge.svg)](https://github.com/deanblackborough/Prune/actions/workflows/build.yml)

# Prune

![Prune](assets/icon.png)

Prune is a C++23 live 2D editor/runtime prototype built with SDL2, SDL2_image, Dear ImGui, and yaml-cpp.

It is not trying to be a traditional engine where editing happens in one mode and gameplay happens in another. The goal is a play-and-build system where the editor remains part of the runtime.

Different scene types can share the same editor shell while defining their own behaviour, tools, panels, inspectors, object semantics, defaults, and save data.

> **Save compatibility warning**
>
> Prune is still pre-release. Existing `.yml` scene files may break while the object model, scene descriptors, behaviour ids, concept metadata, and scene-specific save data are being shaped.
>
> Save compatibility will matter later. For now, the priority is getting the scene model and editor/runtime architecture right.
>
> Camera movements do not mark the scene as dirty but they are saved to the scene file. If you are testing save/load, be aware that camera movements will be persisted.

## Why this exists

My earlier attempt, [prune-2d](https://github.com/deanblackborough/prune-2d), leaned too far towards building generic engine pieces first and hoping the editor and game would appear later.

Prune is take two.

The goal is to build the editor, runtime, and interaction model together from the start. That means solving editor problems early: viewport ownership, input focus, picking, object inspection, save/load, scene-specific behaviour, scene-specific tooling, and the boundary between generic editor tools and scene-owned tools.

## Core idea

Prune is built around a shared editor shell with scene-specific behaviour layered on top.

The shared side owns:

- Viewport
- Grid
- Camera foundations
- Selection
- Outliner
- Generic inspector
- Basic object editing
- Rendering foundations
- Save/load foundations
- Scene creation/loading flow

Each scene type owns:

- Object roles and semantics
- Runtime behaviour
- Scene-specific tools
- Scene-specific panels
- Scene-specific inspector sections
- Scene-specific object creation rules
- Scene-specific default layout
- Scene-specific save data

A simple shooter, platformer, artillery/tank game, card scene, or puzzle scene should be able to reuse most of the editor and general tools whilst still defining the parts that make that particular scene type unique.

The current goal is not to build complete games. The goal is to prove that multiple small game slices can coexist inside the same live editor/runtime without turning the scene layer into one large conditional mess.

As the project develops, the game slices will improve and expand. Tools will be added as scenes need them; as those tools arrive, game slice quality, features, and editor UX will improve.

### Platformer editor and runtime

![Platformer](assets/repo/platformer.png)

### Artillery editor and runtime

![Artillery](assets/repo/artillery.png)

### Simple shooter editor and runtime

![Simple Shooter](assets/repo/simple-shooter.png)

## Prune is not...

Prune is not currently trying to be:

- A general-purpose commercial game engine
- A plugin-based editor framework
- An ECS experiment
- A complete platformer engine
- A complete shooter engine
- A complete artillery engine
- A stable scene file format

The project is still in the phase where concrete slices are being used to discover the right editor/runtime boundaries.

I have a [DECISIONS.md](DECISIONS.md) file which explains some of the choices I have made, and more importantly why I have rejected to do some things.

## Current architecture direction

The project is moving towards this rule:

> A new scene type should only need to define what makes it different.

That means a scene type should not need to reimplement generic viewport access, camera access, object manager access, grid access, generic rendering flow, generic editor interaction, or basic save/load plumbing.

The current architecture is intentionally not a plugin system, not an ECS, and not a general-purpose engine API. The code is still being shaped around concrete scene slices first.

The next important step is explicit authored-object ordering so render order can be edited deliberately and persisted with the scene.

## Where we are now

Prune has moved beyond a single-scene prototype. The project now has a shared editor/runtime foundation with three scene slices proving that different game types can reuse the same editor while owning their own behaviour, semantics, tuning, inspector sections, and save data.

The current focus is no longer "can a scene run?" It is now "can scenes be edited clearly and safely while the runtime remains live?"

Prune currently has:

- Dedicated ImGui scene viewport
- Shared editor camera and game camera foundations
- Grid rendering and snapping
- Live object selection
- Selected-object outlines and transform handles
- Handle-based movement for authored movable objects
- Handle-based scaling for single authored objects, with minimum-size constraints
- Runtime object protection by default
- Outliner and generic inspector panels
- Scene-specific inspector sections
- Scene-aware object concepts (scene roles) for selection, editability, movement, runtime-only objects, and collision meaning
- YAML scene save/load
- Dirty state tracking for authored scene data
- An in-memory authored-object baseline used by save and runtime reset, preserving live editor changes while excluding runtime-only objects
- Scene factory creation and scene-type loading from save files
- Rectangle and sprite rendering
- Basic sprite resource map
- Shared Reset and Pause/Play runtime controls across all three scene slices
- Basic code-driven audio playback and scene event hooks; authored audio UI is deferred until the broader effects/reaction workflow is designed
- Shared scene renderer, interaction, camera, state, collision, serialization, and audio foundations
- Shared `WorldScene` foundation for scene types
- Platformer scene slice
- Artillery game slice
- Simple Shooter scene slice

The first real editor tooling pass was completed when I added the transform gizmo. It was intentionally small: selected authored objects show a visible manipulation affordance, and movement starts from the handle rather than from arbitrary object-body dragging.

This mattered because it shifted Prune from "objects can be edited through panels" towards "the viewport itself is becoming an editor surface" which is what users will expect from a live editor/runtime.

## Editor model

Prune separates shared object editing from scene-specific behaviour.

The generic inspector shows shared object data and scene meaning; the scene-specific inspectors show what that object does in the current scene type.

- Scene meaning: What this object is in the context of the scene, authored or created by the runtime, selectable, editable, the purpose and collision rules.
- Scene type section: What the object is doing in the scene; for example, if it is the player, are they ready to shoot, is there a cooldown, what is the current speed, and so on.

## Current scene slices

The current scenes are proof slices. They are deliberately small, but they exercise different parts of the editor/runtime boundary.

They are not intended to be complete games yet, maybe one day.

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

### Artillery

- Two player
- Individual player state
- Randomised levels (3 defined, random on reset)
- Game reset on tank collision
- Scene-specific tuning panel
- Scene-specific save/load data

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

## Near-term focus

Dirty-state tracking, Ctrl+S, and shared Reset/Pause/Play controls are now implemented. The remaining target in the current hardening phase is explicit authored-object render ordering.

Current priorities:

- [x] Initial implementation of dirty-state tracking for persistent editor-authored changes.
- [x] Keep editor command changes synchronised with the current authored state through execution, undo, redo, save, and runtime reset.
- [x] Add a Ctrl+S shortcut using the existing scene save workflow.
- [x] Add an explicit runtime reset that restores the current in-memory authored state and removes runtime-only objects.
- [x] Add consistent Reset and Pause/Play controls to the editor without marking the scene dirty.
- [ ] Add authored object z-index ordering.
- [ ] Persist and restore object ordering through scene save files.
- [ ] Add basic editor actions for moving selected objects forward or backward in render order.

My development plan is tracked in [NOTES.md](NOTES.md), check the file for more details on what each of these points mean as well as what is included - this is what I will be working on in my next development phase.

## Ready for users when...

The core editor/runtime loop is now working, but that does not mean Prune is ready for you to invest any meaningful time in it just yet.

I will consider Prune ready for early external users when someone unfamiliar with the codebase can create, edit, run, save, reload, and understand a small scene without modifying C++ or relying on undocumented project knowledge.

### Core editor/runtime

- [x] Open the application.
- [x] Select objects in the viewport.
- [x] Move authored objects using the move tool.
- [x] Resize authored objects using the scale tool.
- [x] Duplicate authored objects.
- [x] Delete authored objects.
- [x] Undo and redo editor changes.
- [x] Save and load scenes.
- [x] Reset runtime state without discarding current authored changes.
- [x] Pause and play scene runtime from the shared editor toolbar.
- [x] Preserve scene behaviour after loading.
- [x] Protect runtime-only objects from accidental editing and persistence.
- [x] Provide basic code-driven audio playback and scene audio hooks.

### Editor workflow

- [ ] Select a scene type when creating a new scene.
- [ ] Provide reliable new, open, save, save-as, and saved-scene reload workflows.
- [x] Track unsaved authored changes through editor commands, undo, redo, save, and load.
- [x] Show the current dirty state clearly in the editor.
- [x] Support Ctrl+S for normal scene saving.
- [ ] Warn before closing, reloading, or replacing a scene with unsaved changes.
- [x] Provide consistent Reset and Pause/Play runtime controls.
- [ ] Provide an explicit saved-scene reload workflow.
- [ ] Add explicit authored-object render ordering.
- [ ] Apply grid snapping consistently across applicable editor tools.
- [ ] Provide object locking or protection for authored objects that should not be changed accidentally.

### Scene editing

- [ ] Provide a usable asset workflow that does not require editing source code.
- [ ] Allow sprites and audio resources to be selected and configured through the editor.
- [ ] Expose useful authored-object behaviour through editor controls.
- [ ] Provide editor-facing event and reaction configuration, including audio playback.
- [ ] Support basic scene-wide configuration such as background and music.
- [ ] Add sufficient text rendering support for practical scenes.
- [ ] Introduce scene file versioning before external users create files that need long-term compatibility.

### Proof Prune works and is usable

- [ ] Provide at least one polished example for each supported scene type.
- [ ] Ensure errors such as missing assets and invalid scene files are reported clearly.
- [ ] Document how to build, launch, create, edit, run, save, and reload a scene.
- [ ] Ensure a new user can complete the basic workflow without reading the engine source.
- [ ] Provide at least one practical way to package or share a playable scene. (This may be a simple zip of the scene file and assets, or a small standalone executable.)

*Until all or the majority of the above is checked, please consider Prune an active 2D game engine and editor prototype rather than something you can depend.

## Documentation stance

It is too early for heavy internal documentation.

Useful documentation now:

- [README.md](README.md) explaining the project goal and current architecture direction
- [NOTES.md](NOTES.md) tracking next architecture work
- Short comments where C++ intent is not obvious
- [Scene-type checklist](docs/SCENE_TYPE_CHECKLIST.md)

Too early:

- Full API docs
- Deep class documentation
- Plugin author guide
- Long tutorials

The code is still moving too quickly. Documentation should help decision-making without fossilising unfinished architecture.

## Build

Prune currently targets Windows and is built with CMake, vcpkg, and a C++23-capable compiler.

The project may build on other platforms later, but Windows is the only platform currently verified.

### Confirmed environment
- Windows
- Visual Studio 2022/2026 / MSVC
- CMake
- vcpkg
- C++23
- Ninja or the Visual Studio CMake generator

### Dependencies

Dependencies are managed through vcpkg.json.

Prune currently uses:

- SDL2
- SDL2_image
- Dear ImGui
- yaml-cpp

Dear ImGui is included in the repository under external/, while the remaining third-party libraries are resolved through vcpkg.

### Configure

From the repository root (Powershell examples):

```powershell
cmake -B build -S . `
  -DCMAKE_TOOLCHAIN_FILE=C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake
 ```

### Build
```powershell
cmake --build build
```

For a release build:

```powershell
cmake --build build --config Release
```

### Run

The executable is created in the CMake build output directory. The exact path depends on the generator used.

For Visual Studio generators, the executable will usually be under:

```powershell
build/Release/Prune.exe
```

or:

```powershell
build/Debug/Prune.exe
```

### Notes

- The project is currently developed and tested on Windows.
- The GitHub Actions workflow also builds the project on Windows.
- Other platforms are not intentionally unsupported, but they are not verified yet.
- If CMake cannot find SDL2, SDL2_image, or yaml-cpp, check that the vcpkg toolchain file path is correct.

## Acknowledgements

- [SDL2](https://www.libsdl.org/) — windowing, input, and rendering foundation
- [Dear ImGui](https://github.com/ocornut/imgui) — editor UI
- [yaml-cpp](https://github.com/jbeder/yaml-cpp) — scene save/load
- [SDL2_image](https://github.com/libsdl-org/SDL_image) — sprite loading
- [The Cherno](https://www.youtube.com/@TheCherno) — C++ game engine architecture and editor design inspiration
- [Kenny](https://kenney.nl/) — free game assets for prototyping
- [jsfxr](https://sfxr.me/) - free sound effect generator for prototyping

## Licence

MIT — see [LICENSE](LICENSE)
