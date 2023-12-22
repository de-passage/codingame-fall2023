#pragma once

#include "game_state.hpp"
#include "types.hpp"
#include <iostream>

namespace dpsg {
std::istream &operator>>(std::istream &is, creature_t &c);

creature_list parse_creature_list(std::istream &in);

visible_creature parse_visible_creature(std::istream &in,
                                        const creature_list &creatures);

std::istream &operator>>(std::istream &is, drone_t &d);
visible_creature_list
parse_visible_creature_list(std::istream &in, const creature_list &creatures);
drone_scan parse_drone_scan(std::istream &in, const drone_list &drones,
                            const creature_list &creatures);
input_game_state parse_game_state(std::istream &in, const creature_list &creatures);
} // namespace dpsg
