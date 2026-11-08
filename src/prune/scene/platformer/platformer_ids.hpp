#pragma once

namespace prune::platformer_ids {

  inline constexpr const char* player_behaviour = "platformer.player";
  inline constexpr const char* player_start_behaviour =
      "platformer.player_start";
  inline constexpr const char* platform_behaviour = "platformer.platform";
  inline constexpr const char* hazard_behaviour = "platformer.hazard";

  // Authored concept ids (GameObject::semantics.concept_id). Share the same
  // values as the behaviour ids above today, but are a separate identity:
  // see ObjectSemantics in game_object.hpp.
  inline constexpr const char* player_concept = "platformer.player";
  inline constexpr const char* player_start_concept = "platformer.player_start";
  inline constexpr const char* platform_concept = "platformer.platform";
  inline constexpr const char* hazard_concept = "platformer.hazard";

} // namespace prune::platformer_ids
