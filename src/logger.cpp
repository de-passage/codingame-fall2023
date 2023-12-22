#include "logger.hpp"
#include "format.hpp"

namespace dpsg {
logger::level logger::log_level = logger::level::info;

const logger &operator<<(const logger &l, std::ostream &(*f)(std::ostream &)) {
  if (logger::log_active(l.this_level)) {
    std::cerr << f;
  }
  return l;
}

} // namespace dpsg
