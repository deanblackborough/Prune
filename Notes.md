# Notes

## Current focus

Prune now has enough editor and runtime pieces to start testing the real idea:

A live 2D editor/runtime where generic editor tools are shared, but each scene type can define its own behaviour, tools, panels, and inspectors.

The current simple shooter slice is not meant to become a full game yet. It exists to expose where the architecture needs to bend before adding more scene types.

## Immediate cleanup branch

Before adding new features, clean up the current structure.

### Goals

- Rename SandboxScene to SimpleShooterScene
- Save and load SimpleShooterOptions
- Add a runtime pause option
- Move simple shooter behaviour string constants into one place
- Stop runtime cleanup from bypassing GameObjectManager
- Update README and Notes to match the current roadmap

### Why

The project is about to move from one scene type to multiple scene types. The current code mostly supports that direction, but the naming and a few ownership boundaries still reflect the earlier sandbox phase.

## Next feature branch: transform gizmos

The next major editor feature should be basic transform gizmos.

### First version

- Select an object
- Show a move gizmo at the object origin or centre
- Drag on X axis
- Drag on Y axis
- Drag freely from centre handle
- Respect grid snapping
- Only support position at first

### Not yet

- Rotation
- Scale
- Multi-select
- Pivot editing
- Undo/redo integration

### Why

Dragging objects directly proved the viewport interaction model. Gizmos are the next step because they force the editor to formalise tools instead of baking interaction directly into SceneInteraction.

## Next architecture branch: scene type boundary

After gizmos start to exist, introduce a clearer scene type boundary.

Expected direction:

- Generic editor owns:
  - viewport
  - outliner
  - generic inspector
  - grid
  - camera controls
  - save/load shell
  - object selection
  - generic object transforms

- Scene type owns:
  - runtime update
  - scene-specific options
  - scene-specific panels
  - scene-specific inspector sections
  - scene-specific object creation
  - scene-specific serialization data

Do not over-design this before the second scene exists.

## Second scene type

The second scene type should be small and intentionally different from the shooter.

Best candidate:

### Platformer prototype

Why:

- Gravity
- Jumping
- Ground checks
- Different camera expectations
- Different scene-specific settings
- Different inspector needs

Minimum slice:

- Player affected by gravity
- Solid blocks
- Jump
- Horizontal movement
- One platformer-specific settings panel:
  - gravity
  - jump strength
  - move speed

This will quickly show whether the editor shell is genuinely reusable.

## Sprite handling pass

Sprite handling needs a dedicated improvement pass.

### Short-term

- Keep the resource map simple
- Validate sprite keys on load
- Add sprite selection in inspector
- Show missing sprite fallback clearly
- Support facing-based sprite selection or rotation

### Later

- Sprite sheets
- Animation states
- Scene-specific sprite rules
- Asset browser
- Hot reload

## One-to-two month plan

### Week 1

- Cleanup branch
- Rename SandboxScene
- Save/load SimpleShooterOptions
- Add pause
- Centralise behaviour IDs
- Update docs

### Week 2

- Basic move gizmo
- Pull gizmo state out of SceneInteraction
- Keep position-only

### Week 3

- Improve sprite selection
- Add missing sprite fallback handling
- Add facing sprite/rotation decision

### Week 4

- Extract first scene-type seam
- Keep it small
- Do not build plugin architecture yet

### Weeks 5-6

- Add platformer scene prototype
- Reuse generic panels
- Add platformer-specific settings panel
- Identify what must become generic

### Weeks 7-8

- Refine scene-type boundary
- Improve save/load structure
- Add small debug overlays
- Decide whether card/tetris scene or top-down shooter expansion comes next

## Later roadmap

- Transform gizmos
- Rotate and scale tools
- Undo/redo
- Multi-select
- Object duplication improvements
- Scene-specific inspectors
- Scene-specific creation tools
- Better sprite system
- Animation
- Audio hooks
- Debug overlays
- Collision visualisation
- Scene switching
- Multiple scene examples
- Packaging/build polish
- Example projects