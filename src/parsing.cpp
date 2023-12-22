#include "parsing.hpp"

#include "logger.hpp"

namespace dpsg {
std::istream &operator>>(std::istream &is, creature_t &c) {
  return is >> c.creature_id >> c.color >> c.type;
}

creature_list parse_creature_list(std::istream &in) {
  creature_list creatures;
  int creature_count;
  in >> creature_count;
  in.ignore();

  for (int i = 0; i < creature_count; i++) {
    creature_t creature;
    in >> creature;
    creatures.push_back(creature);
  }

  return creatures;
}

visible_creature parse_visible_creature(std::istream &in,
                                        const creature_list &creatures) {
  visible_creature creature;
  int id;
  in >> id >> creature.pos.x >> creature.pos.y >> creature.vx >> creature.vy;
  creature.creature = find_creature_by_id(creatures, id);
  if (creature.creature == nullptr) {
    error << "Could not find creature: " << id << " in: " << std::endl;
    for (auto &c : creatures) {
      error << "\t" << c << std::endl;
    }
  }
  return creature;
}

visible_creature_list
parse_visible_creature_list(std::istream &in, const creature_list &creatures) {
  visible_creature_list visible_creatures;
  int visible_creature_count;
  in >> visible_creature_count;
  in.ignore();

  for (int i = 0; i < visible_creature_count; i++) {
    visible_creatures.push_back(parse_visible_creature(in, creatures));
  }

  return visible_creatures;
}
drone_scan parse_drone_scan(std::istream &in, const drone_list &drones,
                            const creature_list &creatures) {
  drone_scan scan;
  int drone_id;
  int creature_id;
  in >> drone_id >> creature_id;
  in.ignore();
  scan.drone = find_drone_by_id(drones, drone_id);
  scan.creature = find_creature_by_id(creatures, creature_id);
  if (scan.creature == nullptr) {
    error << "Could not find creature: " << creature_id << " in: " << std::endl;
    for (auto &c : creatures) {
      error << "\t" << c << std::endl;
    }
  }
  if (scan.drone == nullptr) {
    error << "Could not find drone: " << drone_id << " in: " << std::endl;
    for (auto &d : drones) {
      error << "\t" << d << std::endl;
    }
  }
  return scan;
}

radar_blip parse_radar_blip(std::istream &in, const drone_list &drones,
                            const creature_list &creatures) {
  radar_blip blip;
  int drone_id;
  int creature_id;

  in >> drone_id >> creature_id;
  in >> blip.radar;
  in.ignore();
  blip.drone = find_drone_by_id(drones, drone_id);
  blip.creature = find_creature_by_id(creatures, creature_id);
  return blip;
}

input_game_state parse_game_state(std::istream &in, const creature_list &creatures) {
  input_game_state state;
  in >> state.my_score;
  in.ignore();
  in >> state.foe_score;
  in.ignore();

  int my_scan_count;
  in >> my_scan_count;
  in.ignore();
  for (int i = 0; i < my_scan_count; i++) {
    int creature_id;
    in >> creature_id;
    in.ignore();
    auto ptr = find_creature_by_id(creatures, creature_id);
    if (ptr) {
      state.my_scans.push_back(ptr);
    } else {
      error << "Could not find creature: " << creature_id
            << " in: " << std::endl;
      for (auto &c : creatures) {
        error << "\t" << c << std::endl;
      }
    }
  }

  int foe_scan_count;
  in >> foe_scan_count;
  in.ignore();
  for (int i = 0; i < foe_scan_count; i++) {
    int creature_id;
    in >> creature_id;
    in.ignore();
    auto ptr = find_creature_by_id(creatures, creature_id);
    if (ptr) {
      state.foe_scans.push_back(ptr);
    } else {
      error << "Could not find creature: " << creature_id
            << " in: " << std::endl;
      for (auto &c : creatures) {
        error << "\t" << c << std::endl;
      }
    }
  }

  int my_drone_count;
  in >> my_drone_count;
  in.ignore();
  for (int i = 0; i < my_drone_count; i++) {
    drone_t dr;
    in >> dr;
    state.my_drones.push_back(dr);
    state.all_drones.push_back(dr);
  }

  int foe_drone_count;
  in >> foe_drone_count;
  in.ignore();
  for (int i = 0; i < foe_drone_count; i++) {
    drone_t dr;
    in >> dr;
    state.foe_drones.push_back(dr);
    state.all_drones.push_back(dr);
  }


  int drone_scan_count;
  in >> drone_scan_count;
  in.ignore();
  for (int i = 0; i < drone_scan_count; i++) {
    state.drone_scans.push_back(
        parse_drone_scan(in, state.all_drones, creatures));
  }

  int visible_creature_count;
  in >> visible_creature_count;
  in.ignore();
  for (int i = 0; i < visible_creature_count; i++) {
    state.visible_creatures.push_back(parse_visible_creature(in, creatures));
  }

  int radar_blip_count;
  in >> radar_blip_count;
  in.ignore();
  for (int i = 0; i < radar_blip_count; i++) {
    state.radar_blips.push_back(
        parse_radar_blip(in, state.my_drones, creatures));
  }

  return state;
}

std::istream &operator>>(std::istream &is, drone_t &d) {
  return is >> d.drone_id >> d.pos.x >> d.pos.y >> d.emergency >> d.battery;
}

}
