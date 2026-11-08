#pragma once

namespace prune::simple_shooter_ids {

  inline constexpr const char* player_behaviour = "simple_shooter.player";
  inline constexpr const char* enemy_behaviour = "simple_shooter.enemy";
  inline constexpr const char* projectile_behaviour =
      "simple_shooter.projectile";
  inline constexpr const char* wall_behaviour = "simple_shooter.wall";
  inline constexpr const char* spawn_behaviour = "simple_shooter.enemy_spawn";

  // Authored concept ids (GameObject::semantics.concept_id). Share the same
  // values as the behaviour ids above today, but are a separate identity:
  // see ObjectSemantics in game_object.hpp.
  inline constexpr const char* player_concept = "simple_shooter.player";
  inline constexpr const char* enemy_concept = "simple_shooter.enemy";
  inline constexpr const char* projectile_concept = "simple_shooter.projectile";
  inline constexpr const char* wall_concept = "simple_shooter.wall";
  inline constexpr const char* spawn_concept = "simple_shooter.enemy_spawn";

} // namespace prune::simple_shooter_ids
