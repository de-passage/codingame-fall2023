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
  drone_list all_drones;

  drone_scan_list drone_scans;
  visible_creature_list visible_creatures;
  radar_blip_list radar_blips;
};

struct blip_position {
  position top_left = {0, 0};
  position botton_right = {MAX_LENGTH, MAX_LENGTH};
  position center = {MAX_LENGTH / 2, MAX_LENGTH / 2};
  creature_ref creature = nullptr;
};

struct blip_by_distance {
  blip_position blip;
  long distance;

  struct comparator {
    constexpr bool operator()(const blip_by_distance &lhs,
                              const blip_by_distance &rhs) const {
      return lhs.distance > rhs.distance;
    }
  };
};

struct persistent_game_state {
  using blips_ordered_by_distance =
      std::priority_queue<blip_by_distance,
                          std::vector<blip_by_distance>,
                          blip_by_distance::comparator>;

  std::vector<blips_ordered_by_distance> closest_unknowns;
  std::unordered_map<int, creature_ref> known_fish;
  std::unordered_map<int, blip_position> blip_positions;
  std::vector<int> carried_data;

  int light_off = 0; // Number of turns with light off


  inline bool is_known(const creature_t &c) const {
    return known_fish.find(c.creature_id) != known_fish.end();
  }

  inline bool is_known(const visible_creature &c) const {
    return is_known(*c.creature);
  }
  void reset();
};

void precompute_state(const game_state &state, persistent_game_state &p, const creature_list& creatures);
std::pair<drone_ref, long> closest_drone_distance(const game_state &g,
                                                  const position &p);
} // namespace dpsg
