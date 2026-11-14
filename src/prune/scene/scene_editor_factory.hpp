#pragma once

#include <memory>

#include "prune/scene/scene_editor.hpp"
#include "prune/scene/scene_factory.hpp"

namespace prune {

  class SceneEditorFactory {
  public:
    [[nodiscard]] static std::unique_ptr<SceneEditor> create(SceneType type);
  };

} // namespace prune
