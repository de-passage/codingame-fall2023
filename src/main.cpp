#include "game_state.hpp"
#include "logger.hpp"
#include "parsing.hpp"
#include "types.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;
using namespace dpsg;

int main(int argc, const char **argv) {
  for (int i = 1; i < argc; ++i) {
    auto arg = string_view{argv[i]};
    if (arg == "--debug") {
      logger::log_level = logger::level::debug;
    } else if (arg == "--info") {
      logger::log_level = logger::level::info;
    } else if (arg == "--warn") {
      logger::log_level = logger::level::warn;
    } else if (arg == "--error") {
      logger::log_level = logger::level::error;
    } else {
      error << "Unknown argument: " << arg << std::endl;
      return 1;
    }
  }
  auto creatures = parse_creature_list(cin);
  persistent_game_state persistent;
  std::vector<position> current_targets;

  // game loop
  while (1) {
    game_state state = parse_game_state(cin, creatures);
    precompute_state(state, persistent, creatures);

    info << "My scans: " << state.my_scans.size() << ';' << std::endl;
    for (auto &c : state.my_scans) {
      debug << "Scanned fish: " << c << std::endl;
    }

    std::unordered_set<int> targeted_creatures;
    for (size_t i = 0; i < state.my_drones.size(); ++i) {
      auto &drone = state.my_drones[i];
      if (current_targets.size() <= i) {
        current_targets.push_back(drone.pos);
      }
      auto current_target = current_targets[i];
      bool need_light = false;
      if (persistent.carried_data.size() > drone.drone_id && persistent.carried_data[drone.drone_id]>0) {
        current_target = {drone.pos.x, 0};
      } else if (distance_squared(drone.pos, current_target) < sq(SCAN_DISTANCE)) {
        while (!persistent.closest_unknowns[drone.drone_id].empty()) {
          auto closest = persistent.closest_unknowns[drone.drone_id].top();
          persistent.closest_unknowns[drone.drone_id].pop();
          if (targeted_creatures.find(closest.blip.creature->creature_id) ==
              targeted_creatures.end()) {
            targeted_creatures.insert(closest.blip.creature->creature_id);
            current_targets[i] = closest.blip.center;

            debug << "Moving toward " << closest.blip.creature->creature_id
                  << " (" << closest.blip.center
                  << ",distance: " << std::sqrt(closest.distance) << ")"
                  << std::endl;
            break;
          }
        }
      } else if (distance_squared(drone.pos, current_target) <
                 sq(LIGHT_SCAN_DISTANCE)) {
        need_light = true;
      }
      cout << move_to(current_target, need_light) << endl;
    }
  }
}
