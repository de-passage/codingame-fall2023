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
std::ostream& operator<<(std::ostream& os, creature_ref b);
std::ostream &operator<<(std::ostream &os, const drone_t &c);
}
