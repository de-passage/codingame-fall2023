#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>
#include <string>

namespace dpsg {

constexpr inline static size_t MAX_LENGTH = 10000;
constexpr inline static long SCAN_DISTANCE = 800;
constexpr inline static long MOVE_DISTANCE = 600;
constexpr inline static long LIGHT_SCAN_DISTANCE = 2000;
constexpr inline static long BATTERY_CHARGE_RATE = 1;
constexpr inline static long BATTERY_DISCHARGE_RATE = 5;
constexpr inline static long BATTERY_CAPACITY = 30;

struct position {
  int x;
  int y;

  inline bool valid() const {
    return x >= 0 && x < MAX_LENGTH && y >= 0 && y < MAX_LENGTH;
  }
};

long distance_squared(const position &a, const position &b);
template<class T>
inline constexpr T sq(T x) { return x * x; }

struct creature_t {
  int creature_id;
  int color;
  int type;
};
using creature_ref = const creature_t *;
using creature_list = std::vector<creature_t>;
using creature_ref_list = std::vector<creature_ref>;

const creature_t *find_creature_by_id(const creature_list &creatures, int id);

struct visible_creature {
  const creature_t *creature;
  position pos;
  int vx;
  int vy;

  int id() const { return creature->creature_id; }
};

using visible_creature_list = std::vector<visible_creature>;

struct drone_t {
  int drone_id;
  position pos;
  int emergency;
  int battery;
};

using drone_ref = const drone_t *;
using drone_list = std::vector<drone_t>;

struct drone_scan {
  const drone_t *drone;
  const creature_t *creature;
};

using drone_scan_list = std::vector<drone_scan>;

struct radar_blip {
  std::string radar;
  const drone_t *drone;
  const creature_t *creature;
};

using radar_blip_list = std::vector<radar_blip>;

const drone_t *find_drone_by_id(const drone_list &drones, int id);

} // namespace dpsg
