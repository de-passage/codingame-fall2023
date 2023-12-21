#include "logger.hpp"
#include "types.hpp"
#include "game_state.hpp"
#include "parsing.hpp"

#include <algorithm>
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

  // game loop
  while (1) {
    game_state state = parse_game_state(cin, creatures);
    precompute_state(state, persistent);
    info << "Visible creatures: " << state.visible_creatures.size()
         << std::endl;
    for (auto &c : state.visible_creatures) {
      debug << "Visible creature: " << c << std::endl;
    }
    info << "My scans: " << state.my_scans.size() << std::endl;
    for (auto &c : state.my_scans) {
      debug << "Scanned fish: " << c << std::endl;
    }

    for (auto &drone : state.my_drones) {
      if (!persistent.unknown_creatures.empty()) {
        auto closest = persistent.unknown_creatures.top();
        cout << move_to(closest.creature.pos) << endl;
      } else {
        cout << wait() << endl;
      }
    }
  }
}
