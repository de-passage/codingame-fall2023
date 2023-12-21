#pragma once

#include "types.hpp"
#include <queue>
#include <unordered_map>

namespace dpsg {
struct game_state {
  int my_score;
  int foe_score;

  creature_ref_list my_scans;
  creature_ref_list foe_scans;

  drone_list my_drones;
  drone_list foe_drones;

  drone_scan_list drone_scans;
  visible_creature_list visible_creatures;
  radar_blip_list radar_blips;
};

struct creature_by_distance {
  visible_creature creature;
  long distance;

  struct comparator {
    constexpr bool operator()(const creature_by_distance &lhs,
                              const creature_by_distance &rhs) const {
      return lhs.distance > rhs.distance;
    }
  };
};

struct blip_position {
  position top_left = {0, 0};
  position botton_right = {MAX_LENGTH, MAX_LENGTH};
  creature_ref creature = nullptr;
};

struct persistent_game_state {
  using creatures_by_distance =
      std::priority_queue<creature_by_distance,
                          std::vector<creature_by_distance>,
                          creature_by_distance::comparator>;
  creature_ref_list unreported_creatures;
  creatures_by_distance unknown_creatures;

  std::unordered_map<int, creature_ref> known_fish;

  inline bool is_known(const creature_t &c) const {
    return known_fish.find(c.creature_id) != known_fish.end();
  }

  inline bool is_known(const visible_creature &c) const {
    return is_known(*c.creature);
  }

  int light_off = 0; // Number of turns with light off

  std::unordered_map<int, blip_position> blip_positions;
};

void precompute_state(const game_state &state, persistent_game_state &p);
std::pair<drone_ref, long> closest_drone_distance(const game_state &g,
                                                  const position &p);
} // namespace dpsg
