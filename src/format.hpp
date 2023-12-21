#pragma once

#include "types.hpp"
#include <iostream>

namespace dpsg {
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

std::ostream &operator<<(std::ostream &os, const position &p);
std::ostream &operator<<(std::ostream &os, const creature_t &c);
std::ostream &operator<<(std::ostream &os, const visible_creature &c);

struct move_to_t {
  position pos;
  bool light = false;
};

struct wait_t {
  bool light = false;
};

move_to_t move_to(position p, bool light = false);
move_to_t move_to(visible_creature ref, bool light = false);
wait_t wait(bool light = false);

std::ostream &operator<<(std::ostream &os, const move_to_t &m);
std::ostream &operator<<(std::ostream &os, const wait_t &w);
}
