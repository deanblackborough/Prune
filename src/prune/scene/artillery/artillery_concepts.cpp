#include "prune/scene/artillery/artillery_concepts.hpp"

#include "prune/scene/artillery/artillery_ids.hpp"

namespace prune::artillery_concepts {

  ObjectKind kind_for(const GameObject& object) noexcept {
    if (object.semantics.concept_id == artillery_ids::tank_concept) {
      return ObjectKind::Tank;
    }

    if (object.semantics.concept_id == artillery_ids::terrain_concept) {
      return ObjectKind::TerrainLine;
    }

    if (object.semantics.concept_id == artillery_ids::projectile_concept) {
      return ObjectKind::Projectile;
    }

    return ObjectKind::Unknown;
  }

  bool is_tank(const GameObject& object) noexcept {
    return kind_for(object) == ObjectKind::Tank;
  }

  bool is_terrain_line(const GameObject& object) noexcept {
    return kind_for(object) == ObjectKind::TerrainLine;
  }

  bool is_projectile(const GameObject& object) noexcept {
    return kind_for(object) == ObjectKind::Projectile;
  }

  ObjectConcept describe_object(const GameObject& object) noexcept {
    switch (kind_for(object)) {
    case ObjectKind::Tank:
      return ObjectConcept{artillery_ids::tank_concept,
                           "Tank",
                           "Turn-based artillery player object.",
                           "Can be hit by an artillery projectile.",
                           false,
                           true,
                           true};

    case ObjectKind::TerrainLine:
      return ObjectConcept{artillery_ids::terrain_concept,
                           "Terrain Line",
                           "Generated authored terrain segment.",
                           "Stops artillery projectiles.",
                           false,
                           true,
                           true};

    case ObjectKind::Projectile:
      return ObjectConcept{artillery_ids::projectile_concept,
                           "Projectile",
                           "Runtime projectile created by firing.",
                           "Destroyed when it hits terrain or a tank.",
                           true,
                           false,
                           false};

    case ObjectKind::Unknown:
    default:
      return object_concepts::fallback_for(object);
    }
  }

} // namespace prune::artillery_concepts
