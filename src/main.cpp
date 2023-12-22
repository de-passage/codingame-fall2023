#include "agent.hpp"
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

struct target {
  position estimated_position;
  int id;
};

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
  computed_game_state persistent;
  std::vector<position> current_targets;
  agent_map agents;

  // game loop
  while (1) {
    input_game_state state = parse_game_state(cin, creatures);
    precompute_state(state, persistent, creatures);

    info([&](auto &out) {
      for (auto &fish : persistent.blip_positions) {
        if (!persistent.is_known(*fish.second.creature)) {
          out << "Fish " << fish.second.creature->creature_id << " is around "
              << fish.second.center << std::endl;
        }
      }
    });

    // Iterate over drones
    for (auto& drone : state.my_drones) {
      auto& agent = agents[drone.drone_id];
      agent.drone = &drone;
      debug << "Processing drone " << drone.drone_id << flush << ": " << drone << std::endl;
      cout << agent.decide(state, persistent) << endl;
    }
  }
}
