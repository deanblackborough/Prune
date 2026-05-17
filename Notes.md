# Notes

## Current focus

Prune now has two working scene slices:

- Simple Shooter
- Platformer

That is enough to stop guessing about the architecture and start fixing what the second slice has exposed.

The immediate priority is not more tools and not another game type. The immediate priority is making the existing two-slice architecture cleaner so the next scene type does not multiply duplication.

## Current assessment

The project is moving in the right direction.

The editor shell is real. The viewport, outliner, inspector, grid, camera, renderer, interaction, object manager, YAML save/load, and scene factory are all useful foundations.

The problem is that the concrete scene classes still carry too much shared plumbing. SimpleShooterScene and PlatformerScene should eventually read mostly as scene-specific code, but right now they still repeat general scene lifecycle, state access, viewport handling, camera access, object manager access, save/load shape, and editor interaction calls.

That is fine for proving the second scene. It should not become the permanent pattern.

## Immediate architecture branch

### Goal

Clean up the architecture now that two scene types exist.

Do this before adding transform gizmos, bigger scene tooling, or a third scene type.

### Non-goals

- Do not build a plugin system.
- Do not build a full ECS.
- Do not rewrite GameObject completely.
- Do not over-document unstable internals.
- Do not make the platformer a full game yet.

## Issues to fix now

### 1. Shared scene shell extraction

SimpleShooterScene and PlatformerScene repeat too much of the same shell work.

Repeated responsibilities include:

- Owning SceneState
- Owning SceneRenderer
- Owning SceneInteraction
- Viewport get/set
- Object manager access
- Grid options access
- Scene options access
- Camera access
- Common update shape
- Common render shape
- Common reset/default lifecycle shape
- Common save/load shape

Suggested direction:

- Introduce a shared scene shell/base that owns common state and editor/runtime plumbing.
- Let concrete scenes implement only scene-specific behaviour, defaults, tools, inspector sections, and serialization data.
- Keep the design boring and explicit.

A good outcome would be that a new scene type does not need to reimplement object manager getters, viewport getters, camera getters, and generic render/update plumbing.

### 2. Clearer editor/runtime boundary

Current scene update flow still mixes these concerns:

- Runtime behaviour update
- Editor interaction update
- Camera follow update
- Viewport input gating

Suggested direction:

- Shared shell owns editor interaction and generic camera/update sequencing.
- Scene type owns runtime update.
- Scene type receives a small context rather than reaching into everything directly where possible.

The boundary should be clear enough that when a future transform gizmo is added, it obviously belongs to the editor/tooling side, not to SimpleShooterScene or PlatformerScene.

### 3. Real scene-specific inspector support

The interface has `draw_scene_inspector(GameObject& selected)`, and Ui calls it after the generic inspector. That is the right seam.

The current implementation is effectively empty.

Suggested direction:

- Keep generic object properties in the generic inspector.
- Add scene-owned sections only when the selected object has a scene-specific role.
- Make these sections explanatory as well as editable.

Examples:

Simple Shooter:

- Player: movement speed, facing, shooting status
- Enemy: enemy speed, respawn role
- Bullet: runtime object, lifetime, velocity
- Block: solid obstacle role

Platformer:

- Player: grounded state, velocity, jump/move tuning
- Ground/platform: solid platform role
- Hazard: causes player reset on overlap

This will make the editor demonstrate why scene-specific inspectors matter.

### 4. Stronger scene object semantics

GameObject is better grouped now, but the meaning of an object is still mostly implied by fields like `runtime.behaviour`, `collision.solid`, colour, and editor flags.

The next pass should make object intent clearer.

Questions to answer:

- Is this an authored object or runtime-spawned object?
- What scene role does it have?
- Is it solid, hazardous, decorative, player-controlled, enemy-controlled, projectile, terrain, or spawn-only?
- Which system is allowed to create it?
- Which system is allowed to delete it?
- Should the editor expose the fields directly or through scene-specific controls?

Important example:

A platformer hazard should work because it has a platformer hazard role/behaviour that the Platformer runtime checks. The red colour is only presentation.

### 5. Tighten Platformer into a cleaner demo slice

The Platformer slice is useful but still feels like an architecture probe.

Improve it before moving on.

Work to consider:

- Better initial level layout
- Better default camera/zoom expectations
- Clear player start/reset point
- Clear hazard placement
- Clear platform names
- Grounded state visible in the platformer panel or inspector
- Hazard behaviour visible in inspector
- Controls updated so A/D/W/Space behaviour is obvious
- Consistent pause behaviour with Simple Shooter
- Sensible tuning values that feel deliberate

This should stay small. The target is a convincing slice, not a complete platformer.

The Simple Shooter should become the clean reference slice for how a scene type is structured. It does not need more features yet; it needs to be clearer, smaller, and more intentional.

### 6. Tighten shooter into a cleaner demo slice

Focus areas:

- Keep shooter-specific behaviour inside the shooter scene/control code, not the generic inspector.
- Make the player, enemy, projectile, wall, and spawn objects explicit concepts instead of relying mostly on colour, flags, or loose behaviour IDs.
- Remove duplicated tuning values from generic UI panels and keep them in the shooter-specific controls.
- Make collision rules obvious:
  - player movement blocked by solid walls
  - bullets hit enemies
  - bullets stop at walls
  - enemies damage or collide with the player only if that behaviour is part of the slice
- Ensure the outliner and inspector describe what each object is in game terms, not just rectangle data.
- Keep the slice deliberately small:
  - one player
  - simple movement
  - one projectile type
  - one enemy type
  - basic spawn/reset loop if needed

### 7. Organise scene types into folders

The scene folder is now too flat.

Suggested direction:

```text
src/prune/scene/core
src/prune/scene/simple_shooter
src/prune/scene/platformer
src/prune/scene/artillery
```

Possible ownership:

`scene/core`:

- Scene interface
- Shared scene shell/base
- SceneState
- SceneCamera
- SceneRenderer
- SceneInteraction
- SceneSerializer
- GameObject
- GameObjectManager
- Collision helpers
- SceneFactory, unless factory moves higher-level later

`scene/simple_shooter`:

- SimpleShooterScene
- SimpleShooterBehaviour
- SimpleShooterFactory
- SimpleShooterSerializer
- SimpleShooterState
- SimpleShooter IDs

`scene/platformer`:

- PlatformerScene
- PlatformerBehaviour
- PlatformerFactory
- PlatformerSerializer
- PlatformerState
- Platformer IDs

This is a structure change only. Do it when the shared shell work starts, not randomly in isolation.

### 8. Duplication in the code

Known duplication areas:

- Concrete scene lifecycle
- Concrete scene state accessors
- Viewport accessors
- Camera accessors
- Save/load wrapper structure
- Spawn-at-view-centre logic
- Scene-specific panel setup shape
- Scene creation menu entries
- Scene type string handling

Suggested direction:

- Remove duplication only when there is a clear shared concept.
- Keep scene-specific code scene-specific.
- Do not abstract behaviour just because two files look similar.

The useful split is not “no duplication at all”. The useful split is “new scene types should only define what makes them different”.

### 9. Reduce new scene boilerplate

Adding a third scene should not mean creating a large pile of files and repeating the same code.

Target pattern for a small scene:

- One scene class
- One state/options file
- One behaviour file if needed
- One factory/defaults file if needed
- One serializer file only if scene-specific save data exists
- One tooling panel only if it has scene-specific controls
- Optional inspector section only if useful

The shared shell should make unused pieces optional.

The project does not need dynamic plugins yet. A static registration/list is enough.

### 10. Documentation level

Some documentation is useful now, but deep documentation is too early.

Useful now:

- README with current goal and architecture direction
- Notes with immediate architecture plan
- A short scene-type checklist after the shared shell exists
- Comments explaining non-obvious C++/SDL/ImGui decisions

Too early now:

- Full architecture guide
- Plugin guide
- Public API docs
- Long tutorials

The architecture is still moving. Document intent and decisions, not every class in detail.

## Next steps

Once the boundaries are cleaner, continue editor tooling.

Good candidates:

- Transform gizmo
- Better sprite picker
- Collision/debug overlays
- Object duplication improvements
- Basic scene-specific creation tools

## Possible third scene: artillery/tank

A simple artillery/tank scene is a good third slice candidate.

Minimum version:

- Two tanks
- Generated line terrain
- Turn state
- Current player indicator
- Angle control
- Power control
- Fire projectile
- Projectile arc
- Collision with terrain or tank
- Reset/restart round

Why this is a strong architecture test:

- It is turn-based, not continuous movement.
- It uses generated terrain.
- It has two controlled actors.
- It needs a very different scene-specific panel.
- It needs aiming/power tooling rather than movement tuning.
- It uses projectile behaviour differently from Simple Shooter.

This is likely a better next architecture test than a card game. A card game is still interesting, but it would force UI and object model questions that are probably too early while the scene shell is still settling.

## Later backlog

- Transform gizmos
- Rotate and scale tools
- Undo/redo
- Multi-select
- Object duplication improvements
- Scene-specific creation tools
- Better sprite system
- Facing/animation support
- Audio hooks
- Collision visualisation
- Runtime object overlays
- Scene switching polish
- Example projects
- Packaging/build polish
