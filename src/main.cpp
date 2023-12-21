#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

constexpr size_t MAX_LENGTH = 10000;

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
  os << "[ ";
  for (size_t i = 0; i < v.size(); ++i) {
    if (i != 0) {
      os << ", ";
    }
    os << v[i];
  }
  return os << " ]";
}

struct position {
  int x;
  int y;

  bool valid() const {
    return x >= 0 && x < MAX_LENGTH && y >= 0 && y < MAX_LENGTH;
  }
};

std::ostream &operator<<(std::ostream &os, const position &p) {
  return os << "{x:" << p.x << " y:" << p.y << "}";
}

long distance_squared(const position &a, const position &b) {
  auto dx = a.x - b.x;
  auto dy = a.y - b.y;
  return dx * dx + dy * dy;
}

struct logger {
  enum class level { debug, info, warn, error };
  static level log_level;
  level this_level;

  constexpr logger(level l) : this_level(l) {}
};

logger::level logger::log_level = logger::level::error;

template <typename T> const logger &operator<<(const logger &l, const T &t) {
  if (l.this_level >= l.log_level) {
    std::cerr << t;
  }
  return l;
}

const logger &operator<<(const logger &l, std::ostream &(*f)(std::ostream &)) {
  if (l.this_level >= l.log_level) {
    std::cerr << f;
  }
  return l;
}

constexpr logger debug(logger::level::debug);
constexpr logger info(logger::level::info);
constexpr logger warn(logger::level::warn);
constexpr logger error(logger::level::error);

/**
 * Score points by scanning valuable fish faster than your opponent.
 **/

struct creature_t {
  int creature_id;
  int color;
  int type;
};
using creature_ref = const creature_t *;
using creature_list = std::vector<creature_t>;
using creature_ref_list = std::vector<creature_ref>;

std::istream &operator>>(std::istream &is, creature_t &c) {
  return is >> c.creature_id >> c.color >> c.type;
}

std::ostream &operator<<(std::ostream &os, const creature_t &c) {
  return os << "creature{id:" << c.creature_id << " color:" << c.color
            << " type:" << c.type << "}";
}

creature_list parse_creature_list(std::istream &in) {
  creature_list creatures;
  int creature_count;
  cin >> creature_count;
  cin.ignore();

  for (int i = 0; i < creature_count; i++) {
    creature_t creature;
    cin >> creature;
    creatures.push_back(creature);
  }

  return creatures;
}

const creature_t *find_creature_by_id(const creature_list &creatures, int id) {
  auto it =
      std::find_if(creatures.begin(), creatures.end(),
                   [id](const creature_t &c) { return c.creature_id == id; });

  if (it == creatures.end()) {
    return nullptr;
  }

  return &*it;
}

struct visible_creature {
  const creature_t *creature;
  position pos;
  int vx;
  int vy;

  int id() const { return creature->creature_id; }
};

std::ostream &operator<<(std::ostream &os, const visible_creature &c) {
  os << "visible_creature{creature:";
  if (c.creature != nullptr) {
    os << *c.creature;
  } else {
    os << "<null>";
  }
  return os << " pos:" << c.pos << " vx:" << c.vx << " vy:" << c.vy << "}";
}

using visible_creature_list = std::vector<visible_creature>;

visible_creature parse_visible_creature(std::istream &in,
                                        const creature_list &creatures) {
  visible_creature creature;
  int id;
  cin >> id >> creature.pos.x >> creature.pos.y >> creature.vx >> creature.vy;
  creature.creature = find_creature_by_id(creatures, id);
  if (creature.creature == nullptr) {
    error << "Could not find creature: " << id << " in: " << std::endl;
    for (auto &c : creatures) {
      error << "\t" << c << std::endl;
    }
  }
  return creature;
}

visible_creature_list
parse_visible_creature_list(std::istream &in, const creature_list &creatures) {
  visible_creature_list visible_creatures;
  int visible_creature_count;
  cin >> visible_creature_count;
  cin.ignore();

  for (int i = 0; i < visible_creature_count; i++) {
    visible_creatures.push_back(parse_visible_creature(in, creatures));
  }

  return visible_creatures;
}

struct drone_t {
  int drone_id;
  int drone_x;
  int drone_y;
  int emergency;
  int battery;
};

using drone_list = std::vector<drone_t>;

std::istream &operator>>(std::istream &is, drone_t &d) {
  return is >> d.drone_id >> d.drone_x >> d.drone_y >> d.emergency >> d.battery;
}

const drone_t *find_drone_by_id(const drone_list &drones, int id) {
  auto it = std::find_if(drones.begin(), drones.end(),
                         [id](const drone_t &d) { return d.drone_id == id; });

  if (it == drones.end()) {
    return nullptr;
  }

  return &*it;
}

struct drone_scan {
  const drone_t *drone;
  const creature_t *creature;
};

using drone_scan_list = std::vector<drone_scan>;
drone_scan parse_drone_scan(std::istream &in, const drone_list &drones,
                            const creature_list &creatures) {
  drone_scan scan;
  int drone_id;
  int creature_id;
  cin >> drone_id >> creature_id;
  cin.ignore();
  scan.drone = find_drone_by_id(drones, drone_id);
  scan.creature = find_creature_by_id(creatures, creature_id);
  return scan;
}

struct radar_blip {
  std::string radar;
  const drone_t *drone;
  const creature_t *creature;
};

using radar_blip_list = std::vector<radar_blip>;

radar_blip parse_radar_blip(std::istream &in, const drone_list &drones,
                            const creature_list &creatures) {
  radar_blip blip;
  int drone_id;
  int creature_id;

  cin >> drone_id >> creature_id;
  cin >> blip.radar;
  cin.ignore();
  blip.drone = find_drone_by_id(drones, drone_id);
  blip.creature = find_creature_by_id(creatures, creature_id);
  return blip;
}

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

  std::unordered_map<int, creature_ref> known_fish;

  void precompute_state() {
    for (auto &c : my_scans) {
      known_fish.emplace(c->creature_id, c);
    }

    for (auto &c : known_fish) {
      debug << "Known fish: " << *c.second << std::endl;
    }
  }

  bool is_known(const creature_t &c) const {
    return known_fish.find(c.creature_id) != known_fish.end();
  }

  bool is_known(const visible_creature &c) const {
    return is_known(*c.creature);
  }
};

game_state parse_game_state(std::istream &in, const creature_list &creatures) {
  game_state state;
  cin >> state.my_score;
  cin.ignore();
  cin >> state.foe_score;
  cin.ignore();

  int my_scan_count;
  cin >> my_scan_count;
  cin.ignore();
  for (int i = 0; i < my_scan_count; i++) {
    int creature_id;
    cin >> creature_id;
    cin.ignore();
    auto ptr = find_creature_by_id(creatures, creature_id);
    if (ptr) {
      state.my_scans.push_back(ptr);
    } else {
      error << "Could not find creature: " << creature_id
            << " in: " << std::endl;
      for (auto &c : creatures) {
        error << "\t" << c << std::endl;
      }
    }
  }

  int foe_scan_count;
  cin >> foe_scan_count;
  cin.ignore();
  for (int i = 0; i < foe_scan_count; i++) {
    int creature_id;
    cin >> creature_id;
    cin.ignore();
    auto ptr = find_creature_by_id(creatures, creature_id);
    if (ptr) {
      state.foe_scans.push_back(ptr);
    } else {
      error << "Could not find creature: " << creature_id
            << " in: " << std::endl;
      for (auto &c : creatures) {
        error << "\t" << c << std::endl;
      }
    }
  }

  int my_drone_count;
  cin >> my_drone_count;
  cin.ignore();
  for (int i = 0; i < my_drone_count; i++) {
    drone_t dr;
    cin >> dr;
    state.my_drones.push_back(dr);
  }

  int foe_drone_count;
  cin >> foe_drone_count;
  cin.ignore();
  for (int i = 0; i < foe_drone_count; i++) {
    drone_t dr;
    cin >> dr;
    state.foe_drones.push_back(dr);
  }

  int drone_scan_count;
  cin >> drone_scan_count;
  cin.ignore();
  for (int i = 0; i < drone_scan_count; i++) {
    state.drone_scans.push_back(
        parse_drone_scan(in, state.my_drones, creatures));
  }

  int visible_creature_count;
  cin >> visible_creature_count;
  cin.ignore();
  for (int i = 0; i < visible_creature_count; i++) {
    state.visible_creatures.push_back(parse_visible_creature(in, creatures));
  }

  int radar_blip_count;
  cin >> radar_blip_count;
  cin.ignore();
  for (int i = 0; i < radar_blip_count; i++) {
    state.radar_blips.push_back(
        parse_radar_blip(in, state.my_drones, creatures));
  }

  return state;
}

position closest_unknown_fish(const game_state &s, const position &drone_pos) {
  position current_min = {std::numeric_limits<int>::max(),
                          std::numeric_limits<int>::max()};

  for (auto &c : s.visible_creatures) {
    if (!s.is_known(c)) {
      debug << "Found unknown fish: " << c << std::endl;
      if (distance_squared(drone_pos, c.pos) <
          distance_squared(drone_pos, current_min)) {
        debug <<  "Found closer unknown fish: " << c << std::endl;
        current_min = c.pos;
      }
    }
  }
  info << "Closest unknown fish: " << current_min << std::endl;
  return current_min;
}

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

  // game loop
  while (1) {
    game_state state = parse_game_state(cin, creatures);
    state.precompute_state();

    for (auto &drone : state.my_drones) {

      auto closest =
          closest_unknown_fish(state, {drone.drone_x, drone.drone_y});
      if (closest.valid()) {
        cout << "MOVE " << closest.x << " " << closest.y << " 0" << endl;
      } else {
        cout << "WAIT 0" << endl;
      }
    }
  }
}
