#include "decision.hpp"

namespace dpsg {

move_to_t move_to(position p, bool light) { return move_to_t{p, light}; }

move_to_t move_to(visible_creature ref, bool light) {
  return move_to_t{ref.pos, light};
}

wait_t wait(bool light) { return wait_t{light}; }

std::ostream &operator<<(std::ostream &os, const move_to_t &m) {
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

std::ostream &operator<<(std::ostream &os, const decision &d) {
  std::visit([&](auto &&arg) { os << arg; }, d);
  return os;
}
} // namespace dpsg
