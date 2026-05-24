# Prune dev plan

## Where we are

Prune now has enough evidence to make architectural decisions instead of guessing.

There are two working scene slices:

- Simple Shooter
- Platformer

The second slice has proven the basic idea: different scene types can share the editor shell while owning their own behaviour, tools, inspectors, defaults, and save data.

The next phase should not be another scene type yet. The next phase should make the two existing scene types easier to reason about, easier to extend, and safer to use as examples before more tooling is built on top.

**Save compatibility warning**

>
> During this phase, existing `.yml` scene files are allowed to break.
>
> The object model, scene descriptors, behaviour ids, concept metadata, and scene-specific save data are still being shaped. Do not spend time writing migration code yet.
>
> Once the scene-type model, object semantics, and first real editor tooling pass are stable enough for a prototype release, save compatibility rules can be introduced properly.

## Priorities

> Stronger object semantics first, then targeted duplication removal, then editor tooling.

The main problem exposed by the current code is that objects still rely too much on string behaviours, flags, colour, and scene-local interpretation. Before adding real editor tools, the editor needs a clearer answer to this question:

> What is this object in this scene, and which systems are allowed to manipulate it?

Once that is clearer, the duplicate scene save/load wrappers, scene menu registration, object creation helpers, and inspector wording become easier to clean up without over-abstracting.

## Prune principle

A new scene type should only need to define what makes it different.

It should not need to reimplement generic viewport access, camera access, object manager access, grid access, generic render/update flow, generic save/load plumbing, or generic editor interaction.

Scene-specific code should still own:

- object roles
- behaviour rules
- default object layout
- runtime state/options
- scene-specific inspector sections
- scene-specific creation actions
- scene-specific save data

### Non-goals for this phase

- Do not build a plugin system.
- Do not build an ECS.
- Do not rewrite `GameObject` from scratch.
- Do not add a third scene yet.
- Do not build undo/redo yet.
- Do not create a full public API.
- Do not make either slice into a complete game.

### What is working

- `WorldScene` is the right direction. It has already removed a lot of duplicated scene shell code.
- `SimpleShooterScene` and `PlatformerScene` are now visibly scene-specific rather than fully standalone scenes.
- Scene-specific concepts exist in `simple_shooter_concepts` and `platformer_concepts`.
- Scene-specific factories are useful and should stay.
- Scene-specific serializers are useful and should stay.
- The generic inspector plus scene-specific inspector section is the right model.
- The editor/runtime boundary is much clearer than it was before the second scene existed.

### What is not working

- Object meaning is still stringly typed through `runtime.behaviour`.
- `runtime.behaviour` is doing too many jobs: role, behaviour, collision meaning, inspector meaning, and runtime query key.
- Concept metadata is duplicated in shape across scene types.
- The outliner and inspector still depend on each scene translating objects on demand rather than objects carrying a clearer scene role.
- Save/load wrappers in the concrete scenes are very similar.
- Scene type strings are repeated in scene classes and factory code.
- The scene creation menu is hard-coded in UI code.
- Add-at-view-centre logic is duplicated.
- The scene folder is now too flat.

## Phase 1 — Stronger object semantics

Goal: make object intent explicit enough that editor tools can make sensible decisions.

### Tasks

- [x] Introduce a small shared object concept description type.

Suggested shape:

```cpp
struct ObjectConcept {
    std::string_view id;
    std::string_view label;
    std::string_view purpose;
    std::string_view collision_rule;
    bool runtime_only = false;
    bool selectable = true;
    bool editable = true;
};
```

This does not need to become a plugin API. It is just a shared way for scene types to describe objects.

- [x] Add a scene-level function for concept lookup.

Suggested direction:

```cpp
virtual ObjectConcept object_concept_for(const GameObject& object) const = 0;
```

Then `object_role_label()` can become a small wrapper around this.

- [x] Stop using colour as implicit meaning anywhere in explanations or logic.

Colour remains presentation only.

- [x] Separate object role from runtime behaviour, or at minimum wrap access so the code stops directly comparing raw strings everywhere.

Short-term acceptable option:

```cpp
object.runtime.behaviour == platformer_ids::hazard_behaviour
```

becomes hidden behind:

```cpp
platformer_concepts::is_hazard(object)
```

Longer-term better option:

```cpp
struct SceneObjectBinding {
    std::string role;
    std::string behaviour;
};
```

Do not do the longer-term option unless the small concept metadata pass shows it is needed.

- [x] Make editor permissions derive from object semantics where possible.

Examples:

- runtime projectiles should not be renameable, cloneable, or persistent
- spawn markers can be selectable and movable
- player start markers can be selectable and movable
- generated/runtime enemies can be hidden from normal editing unless explicitly debug-visible

- [x] Update generic inspector wording so it distinguishes generic object data from scene meaning.

Generic inspector:

- transform
- size
- render
- collision flag
- editor flags
- lifecycle state

Scene inspector:

- player
- hazard
- wall
- enemy
- projectile
- spawn marker
- scene-specific meaning

### Complete when

- Selecting any authored object clearly tells you what it means in the active scene.
- Runtime objects are clearly identified as runtime-created.
- Hazards, walls, platforms, projectiles, enemies, and spawn markers do not rely on colour to explain behaviour.
- Scene-specific concept metadata is reusable by inspector, outliner, and future tools.

## Phase 2 — Targeted duplication removal

Goal: remove duplication that now has a proven shared concept.

### Tasks

- [x] Move generic save/load wrapper flow into `WorldScene` if it stays the same for both scenes.

Concrete scenes should ideally only provide:

- scene type id
- scene-specific save node writing
- scene-specific load node reading
- post-load validation/repair

Suggested direction:

```cpp
std::string_view scene_type_id() const noexcept override;
void save_scene_data(YAML::Node& root) const override;
bool load_scene_data(const YAML::Node& root, std::string& error) override;
bool restore_loaded_scene(SceneState& state, std::string& error) override;
```

Keep this boring. Do not make it generic beyond current needs.

- [x] Centralise scene type ids.

Current ids:

- `simple_shooter`
- `platformer`

They should not be repeated in multiple files.

- [x] Replace hard-coded scene menu entries with static scene descriptors.

Suggested descriptor:

```cpp
struct SceneDescriptor {
    SceneType type;
    std::string_view id;
    std::string_view label;
    std::string_view default_file_path;
};
```

This is not dynamic plugin registration. It is just a static list.

- [x] Extract common view-centre spawn helper into `WorldScene`.

Both scenes need “create object around active camera centre, optionally snapped to grid”.

- [x] Remove redundant public accessors from concrete scenes unless another system genuinely needs them.

For example, avoid exposing scene-specific state just because it is convenient.

### Complete when

- Adding a third scene does not require editing unrelated UI menu code in several places.
- Save/load structure is consistent between scene types.
- Scene type ids exist in one place.
- View-centre object creation is shared.

## Phase 3 — Folder organisation

Target time: 2 to 3 days.

Goal: make ownership obvious before the project gets larger.

Suggested structure:

```text
src/prune/scene/core
src/prune/scene/top_down_shooter? (simple_shooter now)
src/prune/scene/platformer
```

Possible ownership:

```text
scene/core
  scene.hpp
  world_scene.hpp/.cpp
  scene_state.hpp
  scene_camera.hpp/.cpp
  scene_descriptor.hpp/.cpp
  scene_renderer.hpp/.cpp
  scene_interaction.hpp/.cpp
  scene_serializer.hpp/.cpp
  scene_factory.hpp/.cpp
  game_object.hpp/.cpp
  game_object_manager.hpp/.cpp
  collision.hpp/.cpp
  player_controller.hpp/.cpp (Do we need this at the core level? Should it be scene specific?)

scene/top_down_shooter (simple_shooter now)
  top_down_shooter_scene.hpp/.cpp
  top_down_shooter_behaviour.hpp/.cpp
  top_down_shooter_concepts.hpp/.cpp
  top_down_shooter_factory.hpp/.cpp
  top_down_shooter_serializer.hpp/.cpp
  top_down_shooter_state.hpp
  top_down_shooter_ids.hpp

scene/platformer
  platformer_scene.hpp/.cpp
  platformer_behaviour.hpp/.cpp
  platformer_concepts.hpp/.cpp
  platformer_factory.hpp/.cpp
  platformer_serializer.hpp/.cpp
  platformer_state.hpp
  platformer_ids.hpp
```

Do this after Phase 2, not before. Moving files first will make useful diffs harder to read.

### Complete when

- Core scene infrastructure is visually separated from scene-specific code.
- CMake is updated.
- Include paths are consistent.
- No behaviour changes are included in the folder move branch.

## Phase 4 — First real editor tooling pass

Goal: start proving Prune is a game editor, not just two scenes with some simple editor panels showing data and allowing minor changes.

### Tooling order

1. Transform gizmo / selected object handles
2. Collision/debug overlays
3. Scene-specific creation actions
4. Better sprite picker

### First target: transform gizmo

Keep it small.

- [ ] Draw a simple selected-object outline/handle overlay in the viewport.
- [ ] Allow moving selected authored objects from a visible handle.
- [ ] Respect `object.editor.movable`.
- [ ] Respect viewport input focus.
- [ ] Keep runtime objects protected by default.
- [ ] Do not add rotate/scale yet.

### Second target: debug overlays

- [ ] Toggle collision bounds.
- [ ] Toggle runtime object markers.
- [ ] Toggle scene role labels above objects.
- [ ] Make overlays generic but role labels scene-specific.

### Third target: scene-specific creation actions

Move beyond raw “Add Wall” / “Add Platform” buttons.

Suggested direction:

- shared creation panel shell
- scene supplies creation actions
- action creates an object using scene factory
- placement uses shared view-centre helper
- created object is selected

### Complete when

- The editor has visible manipulation affordances in the viewport.
- Scene-specific object creation feels like a tool system beginning, not just buttons in a panel.
- Runtime/editor protection rules are visible in actual tool behaviour.

## Phase 5 — Behaviour and save/load review

Goal: deliberate review after semantics and tooling expose the next problems.

### Review areas

- [ ] Input focus and viewport gating.
- [ ] Game camera/editor camera switching.
- [ ] Pause behaviour across scenes.
- [ ] Save/load round trips for both scenes.
- [ ] Runtime object cleanup.
- [ ] Object selection after save/load/new scene.
- [ ] Collision consistency.
- [ ] Scene factory behaviour.
- [ ] Default scene layouts.
- [ ] Naming conventions.

### Complete when

- Both slices still compile and run cleanly.
- New scene, save scene, load scene, pause, select, drag, and inspect are stable.
- No runtime-only object is accidentally persisted.
- The editor does not manipulate protected runtime objects unless explicitly allowed.

## Phase 6 — Third scene proof candidate

Recommended third scene: artillery/tank.

Minimum slice:

- two tanks
- generated simple line terrain
- turn state
- current player indicator
- angle control
- power control
- fire projectile
- projectile arc
- collision with terrain or tank
- reset round

Why this is the right third test:

- It is turn-based, not continuous movement.
- It needs generated scene data.
- It has two player-controlled actors.
- It has a very different scene-specific panel.
- It uses projectiles differently from Simple Shooter.
- It needs scene-specific tools that are not movement tuning.

Do not start with a card game yet. A card scene is interesting, but it will pull the project towards UI layout, hands, decks, zones, drag/drop rules, and data modelling before the world-scene editor tooling is mature.

## Phase 7 — Prototype release preparation

Goal: make Prune presentable as a prototype milestone.

### Tasks

- [ ] Update README screenshots.
- [ ] Update README current-state section.
- [ ] Add a short scene-type checklist.
- [ ] Add a short “how to add a scene type” developer note, but keep it high level.
- [ ] Confirm clean build from scratch.
- [ ] Confirm assets load relative to expected working directory.
- [ ] Confirm default scene files are sensible.
- [ ] Tag as prototype/pre-release, not stable API.

## Later...

- Undo/redo
- Multi-select
- Rotate tool
- Scale tool
- Object duplication workflow
- Scene-specific tool palettes
- Better sprite/resource browser
- Animation/facing support
- Audio hooks
- Runtime object overlay controls
- Packaging/build polish
- Example projects
- Card scene
- Puzzle scene
- More robust scene file versioning
