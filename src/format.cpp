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

move_to_t move_to(position p, bool light) {
  return move_to_t{p, light};
}

move_to_t move_to(visible_creature ref, bool light) {
  return move_to_t{ref.pos, light};
}

wait_t wait(bool light) {
  return wait_t{light};
}

std::ostream &operator<<(std::ostream &os, const move_to_t &m) {
  debug << "MOVE " << m.pos.x << " " << m.pos.y << " " << m.light << std::endl;
  os << "MOVE " << m.pos.x << " " << m.pos.y;
  if (m.light) {
    os << " 1";
  } else {
    os << " 0";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, const wait_t &w) {
  os << "WAIT";
  if (w.light) {
    os << " 1";
  } else {
    os << " 0";
  }
  return os;
}
} // namespace dpsg
