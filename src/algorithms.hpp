#pragma once

#include "types.hpp"

namespace dpsg {

  struct id_matches {
    int id;
    constexpr bool operator()(const creature_t&c) const {
      return c.creature_id == id;
    }
    constexpr bool operator()(creature_ref c) const {
      return c->creature_id == id;
    }
    constexpr bool operator()(const drone_t&c) const {
      return c.drone_id == id;
    }
    constexpr bool operator()(drone_ref c) {
      return c->drone_id == id;
    }
  };


  template<class T, class P>
    decltype(auto) find_if(T&& creatures, P&& pred) {
      using  std::begin; using std::end;
      return std::find_if(begin(creatures), end(creatures), pred);
    }

  template<class T, class P>
    bool all_of(T&& creatures, P&& pred) {
      using  std::begin; using std::end;
      return std::all_of(begin(creatures), end(creatures), pred);
    }

  template<class T, class P>
    bool any_of(T&& creatures, P&& pred) {
      using  std::begin; using std::end;
      return std::any_of(begin(creatures), end(creatures), pred);
    }

}
