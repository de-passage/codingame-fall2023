#pragma once

#include "format.hpp"
#include "types.hpp"
#include "game_state.hpp"
#include "decision.hpp"

#include <variant>

namespace dpsg {
  struct hunting_fish {
    creature_ref fish;
    blip_position position;
  };

  using possible_states = std::variant<hunting_fish>;
  using possible_actions = std::variant<move_to_t, wait_t>;

  struct agent {
    possible_states current_state;
    position current_target;
    drone_ref drone;
    creature_ref targeted_creature;

    decision decide(const input_game_state& input, computed_game_state& computed);
  };

  using agent_map = index_map<agent>;
}
