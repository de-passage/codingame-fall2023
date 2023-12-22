#pragma once

#include "decision.hpp"
#include "format.hpp"
#include "game_state.hpp"
#include "types.hpp"

#include <variant>

namespace dpsg {
struct drifting {};
struct hunting_fish {
  int fish;
  blip_position blip;

  hunting_fish(creature_ref f, blip_position b) : fish(f->creature_id), blip(b) {}
};

struct going_to_surface {};

using possible_states = std::variant<drifting, hunting_fish, going_to_surface>;
using possible_actions = std::variant<move_to_t, wait_t>;

struct agent {
  possible_states current_state;
  drone_ref drone;

  decision decide(const input_game_state &input, computed_game_state &computed);
};

using agent_map = index_map<agent>;
} // namespace dpsg
