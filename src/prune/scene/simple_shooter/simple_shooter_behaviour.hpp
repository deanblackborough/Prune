#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"
#include "prune/scene/simple_shooter/simple_shooter_state.hpp"

namespace prune {

  class SimpleShooterBehaviour {
  public:
    void update(WorldState& state, SceneCamera& camera,
                SimpleShooterState& shooter_state, float dt, const Input& input,
                bool keyboard_input_enabled);

    void reset(WorldState& state, SimpleShooterState& shooter_state);

    [[nodiscard]] int projectile_count(const WorldState& state) const noexcept;
    [[nodiscard]] int enemy_count(const WorldState& state) const noexcept;

    [[nodiscard]] GameObject*
    enemy_object(WorldState& state,
                 const SimpleShooterState& shooter_state) const noexcept;

    [[nodiscard]] const GameObject*
    enemy_object(const WorldState& state,
                 const SimpleShooterState& shooter_state) const noexcept;

    [[nodiscard]] GameObject*
    enemy_spawn_object(WorldState& state,
                       const SimpleShooterState& shooter_state) const noexcept;

    [[nodiscard]] const GameObject*
    enemy_spawn_object(const WorldState& state,
                       const SimpleShooterState& shooter_state) const noexcept;

  private:
    GameObject*
    player_object(WorldState& state,
                  const SimpleShooterState& shooter_state) const noexcept;

    const GameObject*
    player_object(const WorldState& state,
                  const SimpleShooterState& shooter_state) const noexcept;

    void update_player(WorldState& state, SceneCamera& camera,
                       SimpleShooterState& shooter_state, float dt,
                       const Input& input, bool keyboard_input_enabled);

    void move_object(WorldState& state, GameObject& object, float delta_x,
                     float delta_y, bool resolve_wall_collisions);
    void update_player_facing(GameObject& player) const noexcept;

    void handle_player_shooting(WorldState& state,
                                SimpleShooterState& shooter_state,
                                const Input& input,
                                bool keyboard_input_enabled);

    void update_enemies(WorldState& state,
                        const SimpleShooterState& shooter_state, float dt);
    void update_projectiles(WorldState& state, float dt);
    void handle_projectile_wall_collisions(WorldState& state);
    void handle_projectile_enemy_collisions(WorldState& state);
    void ensure_enemy_count(WorldState& state,
                            const SimpleShooterState& shooter_state);
    void create_runtime_enemy(WorldState& state,
                              const SimpleShooterState& shooter_state);
    void respawn_enemy(WorldState& state,
                       const SimpleShooterState& shooter_state,
                       GameObject& enemy) const noexcept;
    void cleanup_runtime_objects(WorldState& state);
  };

} // namespace prune
