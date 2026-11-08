#include "prune/scene/scene_editor_factory.hpp"

#include "prune/scene/artillery/artillery_editor.hpp"
#include "prune/scene/platformer/platformer_editor.hpp"
#include "prune/scene/simple_shooter/simple_shooter_editor.hpp"

namespace prune {

  std::unique_ptr<SceneEditor> SceneEditorFactory::create(SceneType type) {
    switch (type) {
    case SceneType::Artillery:
      return std::make_unique<ArtilleryEditor>();

    case SceneType::Platformer:
      return std::make_unique<PlatformerEditor>();

    case SceneType::SimpleShooter:
    default:
      return std::make_unique<SimpleShooterEditor>();
    }
  }

} // namespace prune
