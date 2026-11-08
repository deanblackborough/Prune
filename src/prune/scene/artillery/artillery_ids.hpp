#pragma once

namespace prune::artillery_ids {

  inline constexpr const char* tank_behaviour = "artillery.tank";
  inline constexpr const char* terrain_behaviour = "artillery.terrain_line";
  inline constexpr const char* projectile_behaviour = "artillery.projectile";

  // Authored concept ids (GameObject::semantics.concept_id). Share the same
  // values as the behaviour ids above today, but are a separate identity:
  // see ObjectSemantics in game_object.hpp.
  inline constexpr const char* tank_concept = "artillery.tank";
  inline constexpr const char* terrain_concept = "artillery.terrain_line";
  inline constexpr const char* projectile_concept = "artillery.projectile";

} // namespace prune::artillery_ids
