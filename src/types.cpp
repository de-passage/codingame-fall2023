#include "types.hpp"

namespace dpsg {
long distance_squared(const position &a, const position &b) {
  auto dx = a.x - b.x;
  auto dy = a.y - b.y;
  return sq(dx) + sq(dy);
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

const drone_t *find_drone_by_id(const drone_list &drones, int id) {
  auto it = std::find_if(drones.begin(), drones.end(),
                         [id](const drone_t &d) { return d.drone_id == id; });

  if (it == drones.end()) {
    return nullptr;
  }

  return &*it;
}
} // namespace dpsg
