#pragma once

#include <iostream>
#include "format.hpp"

namespace dpsg {
struct logger {
  enum class level { debug, info, warn, error };
  static level log_level;
  level this_level;

  constexpr logger(level l) : this_level(l) {}
};

template <typename T> const logger &operator<<(const logger &l, const T &t) {
  if (l.this_level >= l.log_level) {
    std::cerr << t;
  }
  return l;
}

const logger &operator<<(const logger &l, std::ostream &(*f)(std::ostream &));

constexpr logger debug(logger::level::debug);
constexpr logger info(logger::level::info);
constexpr logger warn(logger::level::warn);
constexpr logger error(logger::level::error);
}
