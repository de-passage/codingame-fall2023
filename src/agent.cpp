#include "agent.hpp"
#include "decision.hpp"
#include "logger.hpp"
#include <cmath>
#include <ostream>
#include <variant>

namespace dpsg {

std::ostream &operator<<(std::ostream &os, const going_to_surface &p) {
  return os << "Go to surface";
}

std::ostream &operator<<(std::ostream &os, const hunting_fish &p) {
  return os << "Hunt fish " << p.fish << " at " << p.blip.center;
}

std::ostream &operator<<(std::ostream &os, const drifting &p) {
  return os << "Drift";
}

std::ostream &operator<<(std::ostream &os, const possible_states &p) {
  std::visit([&](auto &&arg) { os << arg; }, p);
  return os;
}

struct decision_result {
  decision decided;
  possible_states new_state;
};

struct decider {
  const agent &me;
  const input_game_state &input;
  computed_game_state &computed;

  bool in_potential_light_zone(const position& drone) const {
    for (auto v : computed.blip_positions) {
      if (!computed.my_score_board.scanned(*v.second.creature) && distance_squared(v.second.center, drone) <
          sq(LIGHT_SCAN_DISTANCE)) {
        info << "Drone " << me.drone->drone_id
             << " is at light scan distance of " << v.second.center
             << ", turn on the light and keep going" << std::endl;
        return true;
        break;
      }
    }
    return false;
  }

  auto go_to_surface() const -> decision_result {

    return {move_to({me.drone->pos.x, 0}, in_potential_light_zone(me.drone->pos)), going_to_surface{}};
  }

  auto hunt_best_fish() const -> decision_result {
    blip_by_fish_value target;
    auto best_targets = computed.best_targets.at(me.drone->drone_id);
    debug << "Queue has " << best_targets.size() << " elements" << std::endl;
    while (!best_targets.empty()) {
      auto top_of_queue = best_targets.top();
      best_targets.pop();
      if (computed.targeted_creatures.find(
              top_of_queue.value.creature->creature_id) ==
          computed.targeted_creatures.end()) {
        computed.targeted_creatures.insert(
            top_of_queue.value.creature->creature_id);
        target = top_of_queue;


        constexpr auto DANGER = (1400 + 400) / 2;
        if (abs(me.drone->pos.x - target.value.center.x) < DANGER) {
          return {wait(true), drifting{}};
        }

        for (auto &b : computed.blip_positions) {
          if (abs(b.second.center.x - me.drone->pos.x) < DANGER &&
              abs(b.second.center.y - me.drone->pos.y) < DANGER &&
              !computed.my_score_board.scanned(*b.second.creature)) {
            return {wait(true), drifting{}};
          }
        }
        return {move_to(target.value.center),
                hunting_fish{target.value.creature, target.value}};
      } else {
        debug << "Fish " << top_of_queue.value.creature->creature_id
              << " is already targeted" << std::endl;
      }
    }
    return go_to_surface();
  }

  auto operator()(const drifting &current_state) const -> decision_result {
    return hunt_best_fish();
  }

  auto operator()(const going_to_surface &current_state) const
      -> decision_result {
    if (computed.carried_data.at(me.drone->drone_id).competitive_advantage ==
        0) {
      return hunt_best_fish();
    }
    return go_to_surface();
  }

  auto operator()(hunting_fish current_state) const -> decision_result {

    bool need_light = false;
    debug << "Current target: " << current_state.blip << std::endl;

    auto &carried_data = computed.carried_data.at(me.drone->drone_id);
    debug([&](auto &out) {
      out << "Drone " << me.drone->drone_id << " has "
          << carried_data.competitive_advantage << " competitive advantage\n"
          << "based on " << carried_data.data << std::endl;
    });

    if (carried_data.competitive_advantage > 0) {
      info << "Drone " << me.drone->drone_id
           << " wants to bring data to the surface" << std::endl;
      return go_to_surface();
    } else if (distance_squared(me.drone->pos, current_state.blip.center) <
               sq(SCAN_DISTANCE)) {
      info << "Drone " << me.drone->drone_id << " is at scan distance of "
           << current_state.blip.center << " and looking for a new target"
           << std::endl;

      return hunt_best_fish();
    } else if (distance_squared(me.drone->pos, current_state.blip.center) <
                   sq(LIGHT_SCAN_DISTANCE) ||
               me.drone->pos.x >= MAX_LENGTH - LIGHT_SCAN_DISTANCE - 400 ||
               me.drone->pos.y <= LIGHT_SCAN_DISTANCE + 400 ||
               me.drone->pos.y >= MAX_LENGTH - LIGHT_SCAN_DISTANCE - 400) {
      info << "Drone " << me.drone->drone_id << " is at light scan distance of "
           << current_state.blip.center << ", turn on the light and keep going"
           << std::endl;
      need_light = true;
    }

    if (need_light == false) {
      need_light = me.drone->battery > 10 && in_potential_light_zone(me.drone->pos);
    }
    // Update the center of the blip
    auto blip =
        computed.blip_positions.find(current_state.blip.creature->creature_id);
    if (blip == computed.blip_positions.end()) {
      debug << "Blip " << current_state.blip.creature->creature_id
            << " not found" << std::endl;
      return hunt_best_fish();
    }
    debug << "Updating blip center of " << current_state.blip.creature
          << std::endl;
    current_state.blip.center = blip->second.center;
    return {move_to(current_state.blip.center, need_light), current_state};
  };
};

decision agent::decide(const input_game_state &input,
                       computed_game_state &computed) {
  info << "Agent " << drone->drone_id << " is deciding" << std::endl;
  info << "Current state: " << current_state << std::endl;
  auto [r, new_state] =
      std::visit(decider{*this, input, computed}, current_state);
  current_state = new_state;
  info << "Agent " << drone->drone_id << " decided to " << new_state
       << std::endl;
  info << "Command: " << r << std::endl;
  return r;
}
} // namespace dpsg
