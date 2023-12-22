#include "format.hpp"
#include "logger.hpp"

namespace dpsg {
std::ostream &operator<<(std::ostream &os, const position &p) {
  return os << "{x:" << p.x << " y:" << p.y << "}";
}

std::ostream &operator<<(std::ostream &os, const creature_t &c) {
  return os << "creature{id:" << c.creature_id << " color:" << c.color
            << " type:" << c.type << "}";
}

std::ostream &operator<<(std::ostream &os, const drone_t &d) {
  return os << "drone{id:" << d.drone_id << " pos:" << d.pos << " battery:" << d.battery
            << " emergency:" << d.emergency << "}";
}

std::ostream &operator<<(std::ostream &os, const visible_creature &c) {
  os << "visible_creature{creature:";
  if (c.creature != nullptr) {
    os << *c.creature;
  } else {
    os << "<null>";
  }
  return os << " pos:" << c.pos << " vx:" << c.vx << " vy:" << c.vy << "}";
}

std::ostream& operator<<(std::ostream& os, creature_ref b) {
  return os << *b;
}
} // namespace dpsg
