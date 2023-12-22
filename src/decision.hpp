#pragma once

#include "types.hpp"
#include <iostream>
#include <variant>

namespace dpsg {

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

using decision = std::variant<move_to_t, wait_t>;

std::ostream &operator<<(std::ostream &os, const move_to_t &m);
std::ostream &operator<<(std::ostream &os, const wait_t &w);
std::ostream &operator<<(std::ostream &os, const decision &w);
}
