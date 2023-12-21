#include "game_state.hpp"
#include "logger.hpp"
#include "types.hpp"

namespace dpsg {
void compute_blip(const radar_blip& blip, persistent_game_state& p)  {
  blip_position& result = p.blip_positions[blip.creature->creature_id];
  result.creature = blip.creature;
  if (blip.radar == "TL") { // Above to the left
    result.botton_right = {
      .x = std::min(blip.drone->pos.x, result.botton_right.x),
      .y = std::min(blip.drone->pos.y, result.botton_right.y)
    };
  } else if (blip.radar == "TR") { // Above to the right
    result.top_left = {
      .x = std::max(blip.drone->pos.x, result.top_left.x),
      .y = std::min(blip.drone->pos.y, result.top_left.y)
    };
  } else if (blip.radar == "BL") { // Below to the left
    result.botton_right = {
      .x = std::min(blip.drone->pos.x, result.botton_right.x),
      .y = std::max(blip.drone->pos.y, result.botton_right.y)
    };
  } else if (blip.radar == "BR") { // Below to the right
    result.top_left = {
      .x = std::max(blip.drone->pos.x, result.top_left.x),
      .y = std::max(blip.drone->pos.y, result.top_left.y)
    };
  } else {
    error << "Unknown radar: " << blip.radar << std::endl;
  }
}

void precompute_state(const game_state &state, persistent_game_state &p) {
  p.blip_positions.clear();

  for (const auto& b : state.radar_blips) {
    compute_blip(b, p);
  }

  for (auto &c : state.visible_creatures) {
    auto& b = p.blip_positions[c.creature->creature_id];
    b.top_left = c.pos;
    b.botton_right = c.pos;
  }

  for (const auto& b : p.blip_positions) {
    debug << "Creature " << b.first << " is at position: " << b.second.top_left << " - " << b.second.botton_right << std::endl;
  }

  for (auto &c : state.my_scans) {
    p.known_fish.emplace(c->creature_id, c);
  }
  for (auto &c : state.drone_scans) {
    p.known_fish.emplace(c.creature->creature_id, c.creature);
  }
}

std::pair<drone_ref, long> closest_drone_distance(const game_state &state,
                                                  const position &p) {
  long current_min = std::numeric_limits<int>::max();
  drone_ref current_drone = nullptr;

  for (auto &d : state.my_drones) {
    auto distance_sq = distance_squared(d.pos, p);
    if (distance_sq < current_min) {
      current_min = distance_sq;
      current_drone = &d;
    }
  }
  return { current_drone, current_min };
}
} // namespace dpsg
