#pragma once

#include "types.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace dpsg {
struct input_game_state {
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

template<class T, class U>
struct orderable {
  T value;
  U order;

  struct greater_than {
    constexpr bool operator()(const orderable &lhs,
                              const orderable &rhs) const {
      return lhs.order > rhs.order;
    }
  };
  struct less_than {
    constexpr bool operator()(const orderable &lhs,
                              const orderable &rhs) const {
      return lhs.order < rhs.order;
    }
  };
};

struct blip_by_distance : orderable<blip_position, long> {
  using comparator = typename orderable::greater_than;
  using orderable::order;
  using orderable::value;
};

struct blip_by_fish_value : orderable<blip_position, int> {
  using comparator = typename orderable::less_than;
  using orderable::order;
  using orderable::value;
};

struct score_board {
  enum class State {
    unknown = 0,
    scanned = 1,
    captured = 2,
  };

  constexpr static inline auto TYPE_MAX = 3;
  constexpr static inline auto COLOR_MAX = 4;

  State states[TYPE_MAX][COLOR_MAX] = {{State::unknown}};

  inline constexpr bool color_complete(int color) const {
    for (int i = 0; i < COLOR_MAX; ++i) {
      if (states[color][i] == State::unknown) {
        return false;
      }
    }
    return true;
  }

  inline constexpr bool last_of_color(int id, int color) const {
    for (int i = 0; i < TYPE_MAX; ++i) {
      if (i != id && states[i][color] == State::unknown) {
        return false;
      }
    }
    return true;
  }

  inline constexpr bool type_complete(int type) const {
    for (int i = 0; i < TYPE_MAX; ++i) {
      if (states[i][type] == State::unknown) {
        return false;
      }
    }
    return true;
  }

  inline constexpr bool last_of_type(int id, int type) const {
    for (int i = 0; i < COLOR_MAX; ++i) {
      if (i != id && states[type][i] == State::unknown) {
        return false;
      }
    }
    return true;
  }

  inline bool scanned(int type, int color) const {
    return states[type][color] >= State::scanned;
  }
};

template<class T>
using index_map = std::unordered_map<int, T>;

template<class T>
using priority_queue = std::priority_queue<T, std::vector<T>, typename T::comparator>;

struct research_data {
  std::vector<creature_ref> data;
  int competitive_advantage = 0;
};

struct computed_game_state {
  using blips_ordered_by_distance = priority_queue<blip_by_distance>;
  using blips_ordered_by_value = priority_queue<blip_by_fish_value>;

  index_map<blips_ordered_by_distance> closest_unknowns; // Unknown fish ordered by estimated distance relative to each drone, indexed by drone_id
  index_map<creature_ref> known_fish; // Fish that have been scanned by my drones (not necessarily brought to the surface), indexed by creature_id
  index_map<blip_position> blip_positions; // All blips, indexed by creature_id, with their estimated position
  index_map<research_data> carried_data; // Value of data carried by each drone
  std::vector<blip_position> unknown_creatures; // List of unknown creatures
  index_map<blips_ordered_by_value> best_targets; // Unknown fish ordered by estimated value, indexed by drone_id
  std::unordered_set<int> targeted_creatures; // List of creatures targeted by my drones

  score_board my_score_board;
  score_board foe_score_board;

  int light_off = 0; // Number of turns with light off


  inline bool is_known(const creature_t &c) const {
    return known_fish.find(c.creature_id) != known_fish.end();
  }

  inline bool is_known(const visible_creature &c) const {
    return is_known(*c.creature);
  }
  void reset();
};

void precompute_state(const input_game_state &state, computed_game_state &p, const creature_list& creatures);
std::pair<drone_ref, long> closest_drone_distance(const input_game_state &g,
                                                  const position &p);
int fish_value(const creature_t &c, const creature_list &creatures,
               const input_game_state &current_state,
               const computed_game_state &p);
int competitive_advantage(const creature_t& cr, const computed_game_state& st);
  // If the fish is already known, it has no more value
} // namespace dpsg
