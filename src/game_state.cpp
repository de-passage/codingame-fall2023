#include "game_state.hpp"
#include "algorithms.hpp"
#include "logger.hpp"
#include "types.hpp"
#include <cmath>

namespace dpsg {

void computed_game_state::reset() {
  closest_unknowns.clear();
  blip_positions.clear();
  known_fish.clear();
  carried_data.clear();
  unknown_creatures.clear();
  best_targets.clear();
  targeted_creatures.clear();
}

void compute_blip(const radar_blip &blip, computed_game_state &p) {
  blip_position &result = p.blip_positions[blip.creature->creature_id];
  result.creature = blip.creature;
  switch (blip.creature->type) {
  case 0:
    result.top_left.y = std::max(2500, result.top_left.y);
    result.botton_right.y = std::min(5000, result.top_left.y);
    break;
  case 1:
    result.top_left.x = std::max(5000, result.top_left.x);
    result.botton_right.x = std::min(7500, result.top_left.x);
    break;
  case 2:
    result.top_left.x = std::max(5000, result.top_left.x);
    result.botton_right.x = std::min(7500, result.top_left.x);
    break;
  default:
    break;
  }
  if (blip.radar == "TL") { // Above to the left
    result.botton_right = {
        .x = std::min(blip.drone->pos.x, result.botton_right.x),
        .y = std::min(blip.drone->pos.y, result.botton_right.y)};
  } else if (blip.radar == "TR") { // Above to the right
    result.top_left = {.x = std::max(blip.drone->pos.x, result.top_left.x),
                       .y = std::min(blip.drone->pos.y, result.top_left.y)};
  } else if (blip.radar == "BL") { // Below to the left
    result.botton_right = {
        .x = std::min(blip.drone->pos.x, result.botton_right.x),
        .y = std::max(blip.drone->pos.y, result.botton_right.y)};
  } else if (blip.radar == "BR") { // Below to the right
    result.top_left = {.x = std::max(blip.drone->pos.x, result.top_left.x),
                       .y = std::max(blip.drone->pos.y, result.top_left.y)};
  } else {
    error << "Unknown radar: " << blip.radar << std::endl;
  }
  result.center = {.x = (result.top_left.x + result.botton_right.x) / 2,
                   .y = (result.top_left.y + result.botton_right.y) / 2};
}

void precompute_state(const input_game_state &state, computed_game_state &p,
                      const creature_list &creatures) {
  p.reset();

  debug([&](auto &out) {
    out << "My drones: ";
    for (auto &d : state.my_drones) {
      out << d << ", ";
    }
    out << '\n';
  });

  // Compute blip positions
  // That is, the square in which the fish is based on the available data
  for (const auto &b : state.radar_blips) {
    compute_blip(b, p);
  }

  // If the fish is visible, just set the blip to its position
  for (auto &c : state.visible_creatures) {
    auto &b = p.blip_positions[c.creature->creature_id];
    b.top_left = c.pos;
    b.botton_right = c.pos;
    b.center = c.pos;
  }

  debug([&](auto &out) {
    for (const auto &b : p.blip_positions) {
      out << "Creature " << b.first << " is at between: " << b.second.top_left
          << " and " << b.second.botton_right << " (center: " << b.second.center
          << ")" << std::endl;
    }
  });

  // Fill known_fish and fishes_scanned_by_anyone
  for (auto &c : state.my_scans) {
    p.known_fish.emplace(c->creature_id, c);
    p.my_score_board.states[c->type][c->color] = score_board::State::captured;
  }

  for (auto &c : state.foe_scans) {
    p.foe_score_board.states[c->type][c->color] = score_board::State::captured;
  }

  // Fill carried_data, known_fish
  for (auto &c : state.my_drones) {
    p.carried_data.emplace(c.drone_id, research_data{});
  }
  for (auto &c : state.drone_scans) {
    // If the drone is mine
    if (any_of(state.my_drones, id_matches{c.drone->drone_id})) {
      // add it to the list of known fish, so we don't hunt it again
      p.known_fish.emplace(c.creature->creature_id, c.creature);
      p.my_score_board.states[c.creature->type][c.creature->color] =
          score_board::State::scanned;

      // add it to the list of carried data
      auto id = c.drone->drone_id;
      auto &d = p.carried_data[id];
      d.data.push_back(c.creature);

    } else { // if the drone is the foe's
      p.foe_score_board.states[c.creature->type][c.creature->color] =
          score_board::State::scanned;
    }
  }

  // Fill unknown_creatures
  for (auto &c : creatures) {
    if (!p.is_known(c)) {
      auto it = p.blip_positions.find(c.creature_id);
      if (it != p.blip_positions.end()) {
        p.unknown_creatures.push_back(it->second);
      }
    }
  }

  // Fill closest_unknowns
  for (size_t i = 0; i < state.my_drones.size(); ++i) {
    p.best_targets.emplace(state.my_drones[i].drone_id,
                           computed_game_state::blips_ordered_by_value{});
  }
  for (auto &c : p.unknown_creatures) {
    for (size_t i = 0; i < state.my_drones.size(); ++i) {
      auto d = distance_squared(state.my_drones[i].pos, c.center);
      p.closest_unknowns[i].push(blip_by_distance{{.value = c, .order = d}});
      auto v = fish_value(*c.creature, creatures, state, p);
      p.best_targets[i].push(blip_by_fish_value{{.value = c, .order = v}});
      debug << "Unknown " << c.creature->creature_id << " at: " << c.center
            << " distance is " << std::sqrt(d) << " value is " << v
            << std::endl;
    }
  }

  // Fill competitive advantages
  debug << "Computing competitive advantages" << std::endl;
  for (auto &d : p.carried_data) {
    debug << " -> Drone " << d.first;
    for (auto &c : d.second.data) {
      debug << c << ": ";
      auto ca = competitive_advantage(*c, p);
      d.second.competitive_advantage += ca;
      debug << ca << ", ";
    }
    debug << std::endl;
  }
}

std::pair<drone_ref, long> closest_drone_distance(const input_game_state &state,
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
  return {current_drone, current_min};
}

constexpr inline static auto TYPE_COMPLETION_BONUS = 4;
constexpr inline static auto COLOR_COMPLETION_BONUS = 3;

int fish_value(const creature_t &c, const creature_list &creatures,
               const input_game_state &current_state,
               const computed_game_state &p) {
  // If the fish is already known, it has no more value
  if (p.my_score_board.scanned(c.type, c.color)) {
    return 0;
  }

  auto base_value = c.type + 1;

  // If the fish is not scanned by anyone, its value is doubled
  if (!p.foe_score_board.scanned(c.type, c.color)) {
    base_value *= 2;
  }

  // If this is the last fish of its type, plus 4
  if (p.my_score_board.last_of_type(c.creature_id, c.type)) {
    base_value += TYPE_COMPLETION_BONUS;
    // If the foe hasn't completed this type, bonus is doubled
    if (!p.foe_score_board.type_complete(c.type)) {
      base_value += TYPE_COMPLETION_BONUS;
    }
  }

  // If this is the last fish of its color, plus 3
  if (p.my_score_board.last_of_color(c.creature_id, c.color)) {
    base_value += COLOR_COMPLETION_BONUS;
    // If the foe hasn't completed this color, bonus is doubled
    if (!p.foe_score_board.color_complete(c.color)) {
      base_value += COLOR_COMPLETION_BONUS;
    }
  }

  return base_value;
}

int competitive_advantage(const creature_t &cr, const computed_game_state &st) {
  auto total = 0;
  if (!st.foe_score_board.scanned(cr.type, cr.color)) {
    total += cr.type + 1;
  }
  if (!st.foe_score_board.type_complete(cr.type)) {
    total += TYPE_COMPLETION_BONUS;
  }
  if (!st.foe_score_board.color_complete(cr.color)) {
    total += COLOR_COMPLETION_BONUS;
  }
  return total;
}

} // namespace dpsg
