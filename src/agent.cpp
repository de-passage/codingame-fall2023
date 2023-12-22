#include "agent.hpp"
#include "decision.hpp"
#include "logger.hpp"
#include <cmath>

namespace dpsg {
decision agent::decide(const input_game_state &input,
                       computed_game_state &computed) {
  bool need_light = false;
  info << "Agent " << drone->drone_id << " is deciding" << std::endl;
  debug << "Current target: " << current_target << std::endl;

  auto &carried_data = computed.carried_data.at(drone->drone_id);
  debug([&](auto &out) {
    out << "Drone " << drone->drone_id << " has "
        << carried_data.competitive_advantage
        << " competitive advantage\n"
        << "based on " << carried_data.data
        << std::endl;
  });
  if (carried_data.competitive_advantage > 0) {
    info << "Drone " << drone->drone_id << " wants to bring data to the surface"
         << std::endl;
    current_target = {drone->pos.x, 0};
  } else if (distance_squared(drone->pos, current_target) < sq(SCAN_DISTANCE)) {
    info << "Drone " << drone->drone_id << " is at scan distance of "
         << current_target << " and looking for a new target" << std::endl;

    auto best_targets = computed.best_targets.at(drone->drone_id);
    while (!best_targets.empty()) {
      auto closest = best_targets.top();
      best_targets.pop();
      if (computed.targeted_creatures.find(closest.value.creature->creature_id) ==
          computed.targeted_creatures.end()) {
        computed.targeted_creatures.insert(closest.value.creature->creature_id);
        current_target = closest.value.center;

        info << "Moving toward " << closest.value.creature->creature_id << " ("
             << closest.value.center
             << ",distance: " << std::sqrt(closest.order) << ")" << std::endl;
        break;
      }
    }
  } else if (distance_squared(drone->pos, current_target) <
             sq(LIGHT_SCAN_DISTANCE)) {
    info << "Drone " << drone->drone_id << " is at light scan distance of "
         << current_target << ", turn on the light and keep going" << std::endl;
    need_light = true;
  }

  return move_to(current_target, need_light);
}
} // namespace dpsg
