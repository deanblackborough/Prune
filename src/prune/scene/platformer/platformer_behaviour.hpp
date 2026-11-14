#pragma once

#include "prune/core/input.hpp"
#include "prune/scene/game_object.hpp"
#include "prune/scene/platformer/platformer_state.hpp"
#include "prune/scene/scene_camera.hpp"
#include "prune/scene/scene_state.hpp"

namespace prune {

  class PlatformerBehaviour {
  public:
    void update(WorldState& state, SceneCamera& camera,
                PlatformerState& platformer_state, float dt, const Input& input,
                bool keyboard_input_enabled);

    void reset_player(WorldState& state,
                      PlatformerState& platformer_state) const noexcept;

  private:
    [[nodiscard]] GameObject*
    player_object(WorldState& state,
                  const PlatformerState& platformer_state) const noexcept;
    [[nodiscard]] const GameObject*
    player_object(const WorldState& state,
                  const PlatformerState& platformer_state) const noexcept;
    [[nodiscard]] const GameObject*
    player_start_object(const WorldState& state,
                        const PlatformerState& platformer_state) const noexcept;

    void update_player(WorldState& state, SceneCamera& camera,
                       PlatformerState& platformer_state, float dt,
                       const Input& input, bool keyboard_input_enabled) const;
    void move_player_axis(WorldState& state, GameObject& player, float delta_x,
                          float delta_y, bool& grounded) const;
    [[nodiscard]] bool
    is_resting_on_solid(const WorldState& state,
                        const GameObject& player) const noexcept;
    [[nodiscard]] bool touches_hazard(const WorldState& state,
                                      const GameObject& player) const noexcept;
  };

} // namespace prune
