#include "logger.hpp"
#include "format.hpp"

namespace dpsg {
logger::level logger::log_level = logger::level::debug;

const logger &operator<<(const logger &l, std::ostream &(*f)(std::ostream &)) {
  if (l.this_level >= l.log_level) {
    std::cerr << f;
  }
  return l;
}

} // namespace dpsg
