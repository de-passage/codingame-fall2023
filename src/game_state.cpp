#include "game_state.hpp"
#include "logger.hpp"
#include "types.hpp"
#include <cmath>

namespace dpsg {

void persistent_game_state::reset() {
  closest_unknowns.clear();
  blip_positions.clear();
  known_fish.clear();
  carried_data.clear();
}

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
  result.center = {
    .x = (result.top_left.x + result.botton_right.x) / 2,
    .y = (result.top_left.y + result.botton_right.y) / 2
  };
}

void precompute_state(const game_state &state, persistent_game_state &p, const creature_list& creatures) {
  p.reset();

  for (const auto& b : state.radar_blips) {
    compute_blip(b, p);
  }

  for (auto &c : state.visible_creatures) {
    auto& b = p.blip_positions[c.creature->creature_id];
    b.top_left = c.pos;
    b.botton_right = c.pos;
  }

  for (const auto& b : p.blip_positions) {
    debug << "Creature " << b.first << " is at between: " << b.second.top_left << " and " << b.second.botton_right << " (center: " << b.second.center << ")" << std::endl;
  }

  for (auto &c : state.my_scans) {
    p.known_fish.emplace(c->creature_id, c);
  }
  for (auto &c : state.drone_scans) {
    p.known_fish.emplace(c.creature->creature_id, c.creature);
    auto id = c.drone->drone_id;
    if (p.carried_data.size() <= id) {
      debug << "Resizing carried_data to " << id + 1 << std::endl;
      p.carried_data.resize(id + 1, 0);
    }
    p.carried_data[id] += c.creature->type;
  }

  std::vector<blip_position> unknown_creatures;
  for (auto &c : creatures) {
    if (!p.is_known(c)) {
      auto it = p.blip_positions.find(c.creature_id);
      if (it != p.blip_positions.end()) {
        unknown_creatures.push_back(it->second);
      }
    }
  }

  for (auto &c : unknown_creatures) {
    for (size_t i = 0; i < state.my_drones.size(); ++i) {
      auto d = distance_squared(state.my_drones[i].pos, c.center);
      debug << "Unknown " << c.creature->creature_id << " at: " << c.center << " distance is " << std::sqrt(d) << std::endl;
      p.closest_unknowns[i].push(blip_by_distance{ .blip = c, .distance = d });
    }
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
