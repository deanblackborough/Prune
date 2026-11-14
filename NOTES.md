# Status

Two phases are now complete:

- **Short-term editor/runtime hardening** — dirty-state tracking, Ctrl+S, shared Reset/Pause/Play
  controls, and authored-object z-index ordering.
- **Architecture refactor** — made the seam between authored state, live runtime state, and
  editor/session state explicit and enforced: the authored store is now the single source of
  truth for saved data, `EditorCommand` carries typed payloads instead of a type enum plus
  optional fields, objects have an explicit authored concept identity independent of runtime
  behaviour, editor presentation (Dear ImGui, tooling, scene editors) is structurally separated
  from runtime/scene code down to the CMake target level (`prune_runtime` / `prune_editor`), and
  `WorldScene` was reviewed for further cohesive extraction (the command-application +
  authored-state cluster came out as `AuthoredStore`).

The durable decisions that came out of the refactor — why the authored store is authoritative,
why editing is gated to a Stopped state, why commands are typed, why the editor/runtime split
exists — are recorded in [DECISIONS.md](DECISIONS.md), not here. This file is about what's next.

## Next up

Feature work resumes. Roughly in this order, since each later item leans on something earlier:

1. **Rotation** — the typed `EditorCommand` payloads mean this lands as a label plus reuse of
   the existing `ObjectChange` payload, not a new command type.
2. Background image support
3. Text rendering
4. Behaviour toggles for authored objects — unblocked by explicit object concept identity.
5. Authored event management and event-driven reactions
6. Scene layering, rendering, and collision
7. Grouping
8. Prefabs/templates
9. Asset browser
10. Playable export (native and WebAssembly) — architecturally unblocked now that
    `prune_runtime` has no editor/ImGui dependency.

## Carried over, feature-triggered

Two follow-ups from the architecture refactor were deliberately deferred rather than done now,
because they only make sense once the feature that needs them actually exists:

* **Concept policy vs. authored locks.** Editability is currently gated by
  `object.editor.selectable` (serialized), `ObjectConcept::selectable` / `editable` /
  `runtime_only`, and `identity.type != GameObjectType::Runtime`. Before "Object locking and
  protection" (below) is built, decide whether an operation is forbidden because the *scene
  concept* forbids it, or because *this authored object* was locked by the author — and keep
  those two policies conceptually distinct rather than letting the lock feature reuse/overload
  the existing selectability machinery. Trigger: starting that feature.
* **`GameObject` authored vs. runtime field ownership.** `GameObject` mixes authored data
  (`transform`, `size`, `render`, `collision`) and transient runtime data (`motion.velocity`,
  `motion.facing`, `lifecycle.remaining`) in one struct. Make that ownership explicit before
  Behaviour toggles, Authored events, Grouping, or Prefabs (above) build relationships on top of
  it. Do not pre-emptively redesign `GameObject` or reach for an ECS. Trigger: starting any one
  of those four.

## Build, portability, and CI

Prune builds and is verified on Windows/MSVC and on Linux (GCC 13, Clang 18); this is a deliberate
credibility signal as well as a way to keep lifetime/template bugs that MSVC tolerates from
hiding.

Done:

* Linux build with GCC 13 and Clang 18 alongside Windows/MSVC (app + tests, system dependencies),
  with `build.yml` running a `windows-msvc` + `linux` (`gcc` / `clang`) matrix. CI pins
  `ubuntu-24.04` with explicit `gcc-13` / `clang-18` packages so the documented versions are
  guaranteed.
* AddressSanitizer + UndefinedBehaviorSanitizer build (`PRUNE_SANITIZE`), with the test suite run
  under it in CI — ASan on Windows and Linux, UBSan on the Linux/Clang leg.
* `.clang-format` config enforced in CI.
* The portability issues the Linux port surfaced were near-clean and are fixed: two
  `ImGui::TextWrapped(runtime_string)` format-string-security calls, one `switch` missing a case.
  `std::filesystem` use was already portable; SDL include paths resolved unchanged under system
  packages.
* Object lifetime and pointer-validity discipline (ids as the durable handle, pointers as
  frame-local) is documented as a decision in DECISIONS.md.
* `prune_runtime` / `prune_editor` CMake target split, so "runtime must not depend on editor
  presentation" is a build failure, not just a convention — see DECISIONS.md.

Pending:

* A scripted headless run of the app under sanitizers. The `prune_runtime` target (no ImGui)
  this needed is done; it still needs a smoke/headless mode or a dummy SDL video driver.
* A `clang-tidy` pass — better now that the architecture has settled.

## Medium term, in no particular order

* Rotate tool
* Background image support
* Simplified asset management
* Text rendering
* Behaviour toggles for authored objects
* Polished sample scenes
* Authored event management and event-driven reactions (configure the existing audio path, spawn objects, play animation, screen effects, etc.)
* Asset browser
* Animated sprites/facing support
* Scene file versioning
* Grid snapping support on all tools and UI option, not in settings
* Object locking and protection (see "Carried over" above before starting this)
* More robust collision shapes and collision options
* Scene layering, rendering and collision etc.

## Long term, in no particular order

* Grouping (see "Carried over" above before starting this)
* Input mapping and rebinding
* Scene settings panel for background, music, and other scene-wide options
* Game UI panels
* Pathfinding support
* Full audio mixer
* Prefabs/templates (see "Carried over" above before starting this)
* Native playable export
* WebAssembly playable export
* Card scene
* Puzzle scene
* Many more scene tools
