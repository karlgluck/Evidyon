//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
//                                                                           //
//  Permission is hereby granted, free of charge, to any person obtaining a  //
//  copy of this software and associated documentation files (the            //
//  "Software"), to deal in the Software without restriction, including      //
//  without limitation the rights to use, copy, modify, merge, publish,      //
//  distribute, sublicense, and/or sell copies of the Software, and to       //
//  permit persons to whom the Software is furnished to do so, subject to    //
//  the following conditions:                                                //
//                                                                           //
//  The above copyright notice and this permission notice shall be included  //
//  in all copies or substantial portions of the Software.                   //
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF               //
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.   //
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY     //
//  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,     //
//  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        //
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                   //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#include "map.h"
#include "globalworldmanager.h"
#include "../common/null.h"
#include "shared/evidyon_map.h"
#include "globalgamefilemanager.h"
#include "globalclientmanager.h"
#include "globalactormanager.h"
#include "server/lifeformai/globallifeformaimanager.h"

#include <math.h>
#include <dc/debug>
#include <set>
#include "../common/safemacros.h"

#include "trigger/triggerinstance.h"

// need for GetTickCount
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "common/randint32.h"

#include "server/geosid/globalgeosidmanager.h"
#include "server/geosid/geosidinstance.h"

#include "common/fracture/todo.h"
#include "server/time.h"

namespace Evidyon {


static const unsigned long IMPORTANT_ITEM_DURATION_MILLIS = 5 * 60 * 1000;
static const unsigned long NORMAL_ITEM_DURATION_MILLIS    = 1 * 60 * 1000;





//----[  Map  ]----------------------------------------------------------------
Map::Map() {
  locations_ = NULL;
  map_index_ = 0;
  width_ = 0;
  height_ = 0;
  region_swarm_spawners_ = NULL;
}







//----[  create  ]-------------------------------------------------------------
bool Map::create(World::MapIndex index,
                 World::MapHash hash,
                 int width,
                 int height,
                 GameFile::ServerGameFileMapLocationData* locations,
                 size_t regions_wide,
                 size_t regions_high,
                 GameFile::ServerGameFileMapRegionData* regions,
                 Evidyon::Trigger::Trigger* triggers,
                 size_t number_of_triggers) {

  map_index_ = index;
  map_hash_ = hash;
  { // generate triggers
    triggers_ = new Trigger::Server::TriggerInstance*[number_of_triggers];
    number_of_triggers_ = number_of_triggers;
    for (size_t i = 0; i < number_of_triggers; ++i) {
      triggers_[i]
        = Trigger::Server::TriggerInstance::CreateInstance(&triggers[i]);
    }
  }
  {
    // translate into old-form stuff
    locations_ = new Server::MapLocationInfo[width*height];
    CONFIRM(locations_) else { destroy(); return false; }
    memset(locations_,
           0x00000000,
           sizeof(Server::MapLocationInfo) * width * height);
    Server::MapLocationInfo* map_locations = locations_;
    for (int y = 0; y < height; ++y) {
      for (int x = 0; x < width; ++x) {
        const GameFile::ServerGameFileMapLocationData* source_location
          = &locations[y*width+x];
        Evidyon::World::Navigability
          navigability = source_location->navigability;
        map_locations->navigability = navigability;
        map_locations->zone_rules = source_location->zone_rules;
        map_locations->geonite_burst_cost = source_location->geonite_burst_cost;
        map_locations->geonite_burst_magic = source_location->geonite_burst_magic;
        map_locations->nearby_geosid = source_location->nearby_geosid;
        switch (navigability) {
        using namespace Evidyon::World;
        case NAVIGABILITY_NORMAL:
        case NAVIGABILITY_WADE:
        case NAVIGABILITY_SAFEZONE:
          map_locations->can_move_over = true;
          map_locations->can_fly_over = true;
          map_locations->can_teleport_over = true;
          break;

        case NAVIGABILITY_SWIM:
          map_locations->can_move_over = true;
          map_locations->can_fly_over = true;
          map_locations->can_teleport_over = true;
          break;

        case NAVIGABILITY_NO_TELEPORTING:
          map_locations->can_move_over = true;
          map_locations->can_fly_over = true;
          map_locations->can_teleport_over = false;
          break;
        case NAVIGABILITY_PIT:
          map_locations->can_move_over = true;
          map_locations->can_fly_over = true;
          map_locations->can_teleport_over = true;
          break;
        case NAVIGABILITY_LOWWALL:
          map_locations->can_move_over = false;
          map_locations->can_fly_over = true;
          map_locations->can_teleport_over = true;
          break;
        case NAVIGABILITY_HIGHWALL:
          map_locations->can_move_over = false;
          map_locations->can_fly_over = false;
          map_locations->can_teleport_over = true;
          break;
        case NAVIGABILITY_IMPASSABLE:
          map_locations->can_move_over = false;
          map_locations->can_fly_over = false;
          map_locations->can_teleport_over = false;
          break;

        default:
          //assert(false);
          map_locations->can_move_over = false;
          map_locations->can_fly_over = false;
          map_locations->can_teleport_over = false;
          navigability = World::NAVIGABILITY_IMPASSABLE;
        }

        Trigger::TriggerIndex trigger = source_location->trigger;
        map_locations->trigger =
          (trigger == Trigger::INVALID_TRIGGER_INDEX)
            ? NULL
            : triggers_[trigger];

        map_locations->occluding = source_location->occluding;
        map_locations->chaos_level = source_location->chaos_level;
        map_locations->walking_path[0] = source_location->walking_path[0];
        map_locations->walking_path[1] = source_location->walking_path[1];

        map_locations++;
      }
    }
    width_ = width;
    height_ = height;
  }
  { // initialize the spawn points
    regions_wide_ = regions_wide;
    regions_high_ = regions_high;
    size_t number_of_regions = regions_wide*regions_high;
    region_swarm_spawners_ = new RegionSwarmSpawner[number_of_regions];
    for (size_t y = 0; y < regions_high; ++y) {
      for (size_t x = 0; x < regions_wide; ++x) {
        const size_t index = y*regions_wide+x;
        region_swarm_spawners_[index].create(
          x * World::REGION_SIZE,
          y * World::REGION_SIZE,
          &regions[index]);
      }
    }
  }
//  { // pattern-match to create pathfinding directions
//    for (size_t z = 1; z < height - 1; ++z) {
//      size_t z_base = z * width;
//      size_t z_base_n = z_base - width;
//      size_t z_base_s = z_base + width;
//      for (size_t x = 1; x < width-1; ++x) {
//        size_t x_w = x - 1;
//        size_t x_e = x + 1;
//#define IS_SOLID(n) (n != World::NAVIGABILITY_NORMAL &&\
//                     n != World::NAVIGABILITY_WADE &&\
//                     n != World::NAVIGABILITY_NO_TELEPORTING)
//        if (!IS_SOLID(locations_[z_base + x].navigability)) continue;
//
//        // set up the clockwise paths
//        locations_[z_base_n + x_w].cw_walking_path_direction
//          = Server::MapLocationInfo::EAST;
//        locations_[z_base_n + x].cw_walking_path_direction
//          = Server::MapLocationInfo::EAST;
//        locations_[z_base_n + x_e].cw_walking_path_direction
//          = Server::MapLocationInfo::SOUTH;
//        locations_[z_base + x_w].cw_walking_path_direction
//          = Server::MapLocationInfo::NORTH;
//        locations_[z_base + x].cw_walking_path_direction
//          = Server::MapLocationInfo::NA;
//        locations_[z_base + x_e].cw_walking_path_direction
//          = Server::MapLocationInfo::SOUTH;
//        locations_[z_base_s + x_w].cw_walking_path_direction
//          = Server::MapLocationInfo::NORTH;
//        locations_[z_base_s + x].cw_walking_path_direction
//          = Server::MapLocationInfo::WEST;
//        locations_[z_base_s + x_e].cw_walking_path_direction
//          = Server::MapLocationInfo::WEST;
//
//        // set up the counterclockwise paths
//        locations_[z_base_n + x_w].ccw_walking_path_direction
//          = Server::MapLocationInfo::SOUTH;
//        locations_[z_base_n + x].ccw_walking_path_direction
//          = Server::MapLocationInfo::WEST;
//        locations_[z_base_n + x_e].ccw_walking_path_direction
//          = Server::MapLocationInfo::WEST;
//        locations_[z_base + x_w].ccw_walking_path_direction
//          = Server::MapLocationInfo::SOUTH;
//        locations_[z_base + x].ccw_walking_path_direction
//          = Server::MapLocationInfo::NA;
//        locations_[z_base + x_e].ccw_walking_path_direction
//          = Server::MapLocationInfo::NORTH;
//        locations_[z_base_s + x_w].ccw_walking_path_direction
//          = Server::MapLocationInfo::EAST;
//        locations_[z_base_s + x].ccw_walking_path_direction
//          = Server::MapLocationInfo::EAST;
//        locations_[z_base_s + x_e].ccw_walking_path_direction
//          = Server::MapLocationInfo::NORTH;
//      }
//    }
//
//    // groom the directions so that nothing points into a solid block
//    for (size_t z = 0; z < height; ++z) {
//      size_t z_base = z * width;
//      size_t z_base_n = z_base - width;
//      size_t z_base_s = z_base + width;
//      for (size_t x = 0; x < width; ++x) {
//        size_t x_w = x - 1;
//        size_t x_e = x + 1;
//        { // first, clockwise
//          Server::MapLocationInfo::PathDirection d
//            = locations_[z_base + x].cw_walking_path_direction;
//          while (d != Server::MapLocationInfo::NA) {
//            // check the target direction
//            Server::MapLocationInfo* next = NULL;
//            switch (d) {
//            case Server::MapLocationInfo::NORTH:
//              next = z > 0 ? &locations_[z_base_n + x] : NULL; break;
//            case Server::MapLocationInfo::EAST:
//              next = x+1<width ? &locations_[z_base + x_e] : NULL; break;
//            case Server::MapLocationInfo::SOUTH:
//              next = z+1<height ? &locations_[z_base_s + x] : NULL; break;
//            case Server::MapLocationInfo::WEST:
//              next = x > 0 ? &locations_[z_base + x_w] : NULL; break;
//            }
//            if (!next || IS_SOLID(next->navigability)) {
//              // search counterclockwise
//              d = Server::MapLocationInfo::PathDirection(
//                d == 0 ? Server::MapLocationInfo::WEST : d - 1);
//              if (d == locations_[z_base + x].cw_walking_path_direction) {
//                d = Server::MapLocationInfo::NA; // couldn't find a match
//              }
//            } else {
//              break;
//            }
//          }
//          locations_[z_base + x].cw_walking_path_direction = d;
//        }
//        { // second, counterclockwise
//          Server::MapLocationInfo::PathDirection d
//            = locations_[z_base + x].ccw_walking_path_direction;
//          while (d != Server::MapLocationInfo::NA) {
//            // check the target direction
//            Server::MapLocationInfo* next = NULL;
//            switch (d) {
//            case Server::MapLocationInfo::NORTH:
//              next = z > 0 ? &locations_[z_base_n + x] : NULL; break;
//            case Server::MapLocationInfo::EAST:
//              next = x+1<width ? &locations_[z_base + x_e] : NULL; break;
//            case Server::MapLocationInfo::SOUTH:
//              next = z+1<height ? &locations_[z_base_s + x] : NULL; break;
//            case Server::MapLocationInfo::WEST:
//              next = x > 0 ? &locations_[z_base + x_w] : NULL; break;
//            }
//            if (!next || IS_SOLID(next->navigability)) {
//              // search clockwise
//              d = Server::MapLocationInfo::PathDirection(
//                d == Server::MapLocationInfo::WEST ? Server::MapLocationInfo::NORTH: d + 1);
//              if (d == locations_[z_base + x].ccw_walking_path_direction) {
//                d = Server::MapLocationInfo::NA; // couldn't find a match
//              }
//            } else {
//              break;
//            }
//          }
//          locations_[z_base + x].ccw_walking_path_direction = d;
//        }
//      }
//#undef IS_SOLID
//    }
//  }
  {
    inhabitants_ = NULL;
    number_of_inhabitants_ = 0;
  }
  return true;
}




//----[  destroy  ]------------------------------------------------------------
void Map::destroy() {

  // Free all triggers
  if (triggers_) {
    for (size_t i = 0; i < number_of_triggers_; ++i) {
      SAFE_DELETE(triggers_[i]);
    }
    SAFE_DELETE_ARRAY(triggers_);
  }

  { // Release all regions
    GlobalWorldManager* manager = GlobalWorldManager::singleton();
    for (WorldRegions::iterator i = regions_.begin();
         i != regions_.end(); ++i) {
      WorldRegion* region = i->second;
      manager->deallocateRegion(region);
    }
    regions_.clear();
  }

  if (region_swarm_spawners_ != NULL)
  { // Get rid of the region swarm spawners
    //for (int y = 0; y < regions_high_; ++y) {
    //  for (int x = 0; x < regions_wide_; ++x) {
    //    const int index = regions_wide_ * y + x;
    //    region_swarm_spawners_[index].destroy();
    //  }
    //}
    SAFE_DELETE_ARRAY(region_swarm_spawners_);
  }
  regions_wide_ = 0;
  regions_high_ = 0;
  active_region_swarm_spawners_.clear();

  { // free inhabitants
    SAFE_DELETE_ARRAY(inhabitants_);
    number_of_inhabitants_ = 0;
    world_region_inhabitants_.clear();
  }

  SAFE_DELETE_ARRAY(locations_);
  width_ = 0;
  height_ = 0;
}





//----[  getIndex  ]-----------------------------------------------------------
World::MapIndex Map::getIndex() const{
  return map_index_;
}


//----[  getHash  ]------------------------------------------------------------
World::MapHash Map::getHash() const {
  return map_hash_;
}



//----[  getMapInfo  ]---------------------------------------------------------
Server::MapLocationInfo* Map::getMapInfo() {
  return locations_;
}



//----[  createInhabitants  ]--------------------------------------------------
void Map::createInhabitants(size_t number_of_inhabitants) {
  inhabitants_ = new Inhabitant[number_of_inhabitants];
  number_of_inhabitants_ = number_of_inhabitants;
}



//----[  setInhabitant  ]------------------------------------------------------
void Map::setInhabitant(size_t inhabitant_index, int x, int y, size_t lifeform_ai_type) {
  ASSERT(inhabitant_index < number_of_inhabitants_) else return;
  Inhabitant* inhabitant = &inhabitants_[inhabitant_index];
  inhabitant->x = x;
  inhabitant->y = y;
  inhabitant->lifeform_ai_type = lifeform_ai_type;
  inhabitant->actor.reset();

  world_region_inhabitants_.insert(WorldRegionInhabitants::value_type(World::regionIDFromPoint(x, y), inhabitant));
}




//----[  checkForUnusedMapRegions  ]-------------------------------------------
void Map::checkForUnusedMapRegions() {
  typedef std::set<WorldRegion*> RegionSet;
  RegionSet regions_to_remove;
  for (WorldRegions::iterator i = regions_.begin();
       i != regions_.end(); ++i) {
    WorldRegion* region = i->second;
    if (region->neighboringRegionsHaveNoNearbyClients() &&
        region->containsNoItems()) {
      regions_to_remove.insert(region);
    }
  }

  if (regions_to_remove.empty()) return;

  for (RegionSet::iterator i = regions_to_remove.begin();
       i != regions_to_remove.end(); ++i) {
    destroyRegion(*i);
  }
}




//----[  clearOldItemsOnGround  ]----------------------------------------------
void Map::clearOldItemsOnGround() {
  unsigned long time = GetTickCount();
  while (!clear_items_on_location_queue_.empty()) {
    const ClearItemsOnMapLocation& clear_struct = clear_items_on_location_queue_.top();
    if (time < clear_items_on_location_queue_.top().millisecond_timer_time) break;
    WorldRegion* region = getRegionFromPoint(clear_struct.x, clear_struct.y);

    if (region) {
      region->clearItemsOnMapLocation(&clear_struct);
    }

    clear_items_on_location_queue_.pop();
  }
}




//----[  destroyRegion  ]------------------------------------------------------
void Map::destroyRegion(WorldRegion* region) {
  {
    Server::GlobalLifeformAIManager* lifeform_ai_manager =
        Server::GlobalLifeformAIManager::singleton();
    lifeform_ai_manager->releaseRegion(region);
  }
  {
    for (ActiveRegionSwarmSpawners::iterator i = active_region_swarm_spawners_.begin();
        i != active_region_swarm_spawners_.end(); ++i) {
      RegionSwarmSpawner* spawner = *i;
      if (spawner->deactivateIfRegionIs(region)) {
        active_region_swarm_spawners_.erase(i);
        break;
      }
    }
  }
  { // remove from the internal map
    regions_.erase(region->getRegionID());
  }
  {
    GlobalWorldManager* world_manager = GlobalWorldManager::singleton();
    world_manager->deallocateRegion(region);
  }
}



//----[  updateSpawn  ]--------------------------------------------------------
void Map::updateSpawn(double time) {
  todo("karl", "THIS STRUCTURE IS VERY WEAK!  if an actor is acquired by the "\
               "spawner that causes a new region with other spawners to be acquired, "\
               "the iterator blows up");
  Server::GlobalLifeformAIManager* lifeform_ai_manager
    = Server::GlobalLifeformAIManager::singleton();
  for (ActiveRegionSwarmSpawners::iterator i = active_region_swarm_spawners_.begin();
      i != active_region_swarm_spawners_.end(); ++i) {
    RegionSwarmSpawner* spawner = *i;
    spawner->update(lifeform_ai_manager);
  }
}




//----[  getMapLocationInfo  ]-------------------------------------------------
const Server::MapLocationInfo* Map::getMapLocationInfo(int x, int y) const {
  CONFIRM(x >= 0 && y >=0 && x < width_ && y < height_) else {
    return NULL;
  }
  return &locations_[y*width_ + x];
}



//----[  canCreatePortalAt  ]--------------------------------------------------
bool Map::canCreatePortalAt(Number x, Number z) {
  const Server::MapLocationInfo* location
    = getMapLocationInfo(x.wholePart(), z.wholePart());
  if (!location) return false;
  if (!location->can_move_over ||
      location->trigger ||
      location->zone_rules != Zone::ZONERULES_NORMAL) {
    // can't create portals on triggers, pvp areas, or safe zones
    // portals are not allowed where one cannot move (walls)
    return false;
  }
  Geosid::GeosidIndex geosid = location->nearby_geosid;
  Server::GeosidInstance* instance
    = Server::GlobalGeosidManager::singleton()->getGeosidByIndex(geosid);
  if (instance && instance->isWithinRange(getIndex(), x, z)) {
    // can't create portals within the radius of a geosid
    return false;
  }

  return true;
}



//algorithm from http://playtechs.blogspot.com/2007/03/raytracing-on-grid.html
class RaytraceFunctionoid {
public:
  virtual ~RaytraceFunctionoid() = 0;

  // return 'false' to exit the raytracer
  virtual bool visit(int x, int y) = 0;
};
RaytraceFunctionoid::~RaytraceFunctionoid() {}
void raytrace(int x0, int y0, int x1, int y1,
              RaytraceFunctionoid* functionoid) {
  int dx = abs(x1 - x0);
  int dy = abs(y1 - y0);
  int x = x0;
  int y = y0;
  int n = 1 + dx + dy;
  int x_inc = (x1 > x0) ? 1 : -1;
  int y_inc = (y1 > y0) ? 1 : -1;
  int error = dx - dy;
  dx *= 2;
  dy *= 2;

  for (; n > 0; --n) {
    if (!functionoid->visit(x, y)) return;

    if (error > 0) {
      x += x_inc;
      error -= dy;
    } else {
      y += y_inc;
      error += dx;
    }
  }
}

//Finally: The code above does not always return the same set of squares
// if you swap the endpoints. When error is zero, the line is passing
// through a vertical grid line and a horizontal grid line simultaneously.
// In this case, the code currently will always move vertically (the else
// clause), then horizontally. If this is undesirable, you could make the
// if statement break ties differently when moving up vs. down; or you
// could have a third clause for error == 0 which considers both moves
// (horizontal-then-vertical and vertical-then-horizontal).




//----[  isValidTeleport  ]----------------------------------------------------
bool Map::isValidTeleport(ActorInstance* actor, float dest_x, float dest_y) const {
  Number actor_x, actor_z;
  actor->getPosition(&actor_x, &actor_z);
  return isValidTeleport(actor_x, actor_z, dest_x, dest_y);
}



//----[  isValidTeleport  ]----------------------------------------------------
bool Map::isValidTeleport(Number source_x,
                          Number source_z,
                          Number target_x,
                          Number target_z) const {
  const Server::MapLocationInfo* start_location
    = getMapLocationInfo(source_x.wholePart(), source_z.wholePart());
  const Server::MapLocationInfo* end_location
    = getMapLocationInfo(target_x.wholePart(), target_z.wholePart());
  if (!(end_location ? end_location->can_move_over : false) ||
    ((start_location->zone_rules == Zone::ZONERULES_PVP_ARENA) != 
     (end_location->zone_rules == Zone::ZONERULES_PVP_ARENA))) return false;

  // raytrace to this location
  class LocationFunctionoid : public RaytraceFunctionoid {
  public:
    virtual bool visit(int x, int z) {
      const Server::MapLocationInfo* location
        = map->getMapLocationInfo(x, z);
      completed = location && location->can_teleport_over;
      return completed;
    }
    const Map* map;
    bool completed;
  } functionoid;
  functionoid.map = this;
  functionoid.completed = true;
  raytrace(source_x.wholePart(), source_z.wholePart(),
           target_x.wholePart(), target_z.wholePart(),
           &functionoid);
  return functionoid.completed;
}



//----[  isValidMonsterWalkPath  ]---------------------------------------------
bool Map::isValidMonsterWalkPath(Number source_x,
                                 Number source_z,
                                 Number target_x,
                                 Number target_z) const {
  const Server::MapLocationInfo* location
    = getMapLocationInfo(target_x.wholePart(), target_z.wholePart());
  if (!(location ? location->can_move_over : false)) return false;

  // raytrace to this location
  class LocationFunctionoid : public RaytraceFunctionoid {
  public:
    virtual bool visit(int x, int z) {
      const Server::MapLocationInfo* location
        = map->getMapLocationInfo(x, z);
      if (location) {
        switch (location->navigability) {
        case World::NAVIGABILITY_NORMAL:
        case World::NAVIGABILITY_NO_TELEPORTING:
        case World::NAVIGABILITY_WADE:
          return true;
        }
      }
      completed = false;
      return false;
    }
    const Map* map;
    bool completed;
  } functionoid;
  functionoid.map = this;
  functionoid.completed = true;
  raytrace(source_x.wholePart(), source_z.wholePart(),
           target_x.wholePart(), target_z.wholePart(),
           &functionoid);
  return functionoid.completed;
}



//----[  testLineOfSight  ]----------------------------------------------------
bool Map::testLineOfSight(Number source_x,
                          Number source_z,
                          Number target_x,
                          Number target_z) const {
  // raytrace to this location
  class LocationFunctionoid : public RaytraceFunctionoid {
  public:
    virtual bool visit(int x, int z) {
      const Server::MapLocationInfo* location
        = map->getMapLocationInfo(x, z);
      completed = (location && !location->occluding);
      return completed;
    }
    const Map* map;
    bool completed;
  } functionoid;
  functionoid.map = this;
  functionoid.completed = true;
  raytrace(source_x.wholePart(), source_z.wholePart(),
           target_x.wholePart(), target_z.wholePart(),
           &functionoid);
  return functionoid.completed;
}


//----[  printDebugStats  ]----------------------------------------------------
void Map::printDebugStats(char* buffer, size_t buffer_size) {
  sprintf_s(buffer,
            buffer_size,
            "Map %i (%ix%i regions) has %i active regions, %lu active swarm spawners",
            map_index_,
            regions_wide_,
            regions_high_,
            regions_.size(),
            active_region_swarm_spawners_.size());
}




//----[  validatePortalDestination  ]------------------------------------------
bool Map::validatePortalDestination(Number x,
                                    Number z) {
  const Server::MapLocationInfo* location
    = getMapLocationInfo(x.wholePart(), z.wholePart());
  if (!location) return false;
  return location->can_move_over;
}


//----[  getRandomChaosLocation  ]---------------------------------------------
void Map::getRandomChaosLocation(Number* x,
                                 Number* z) {
  assert(false);
  /*
  unsigned int side = rand_uint32();
  side &= (4 - 1);
  Number coord;
  coord.int_value = rand_uint32() & 0x7FFFFFFF; // no negatives
  Number chaos_x, chaos_z;
  static const int BORDER = 5;
  switch (side) {
    case 0: // top
      chaos_x = coord.wholePart() % width_;
      chaos_z = BORDER;
      break;
    case 1: // right
      chaos_x = width_ - BORDER - 1;
      chaos_z = coord.wholePart() % height_;
      break;
    case 2: // bottom
      chaos_x = coord.wholePart() % width_;
      chaos_z = height_ - BORDER - 1;
      break;
    case 3: // left
      chaos_x = BORDER;
      chaos_z = coord.wholePart() % height_;
      break;
    default:
      assert(false);
      break;
  }

  *x = chaos_x;
  *z = chaos_z;*/
}

//
//void Map::debugFindActor(ActorInstance* actor) {
//  for (WorldRegions::iterator i = regions_.begin(); i != regions_.end(); ++i) {
//    WorldRegion* region = i->second;
//    if (region->debugContainsActor(actor)) {
//      assert(false);
//    }
//  }
//}


//----[  getRegionFromPoint  ]-------------------------------------------------
WorldRegion* Map::getRegionFromPoint(float x, float y) {
  Evidyon::World::RegionID id = Evidyon::World::regionIDFromPoint(x, y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
}




//----[  getRegionFromPoint  ]-------------------------------------------------
WorldRegion* Map::getRegionFromPoint(int x, int y) {
  Evidyon::World::RegionID id = World::regionIDFromPoint(x,y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
}



//----[  acquireRegionFromPoint  ]---------------------------------------------
WorldRegion* Map::acquireRegionFromPoint(float x, float y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  WorldRegions::iterator region = regions_.find(id);
  return region == regions_.end() ? createRegion(region_x, region_y) : region->second;
}



//----[  acquireRegionFromPoint  ]---------------------------------------------
WorldRegion* Map::acquireRegionFromPoint(int x, int y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  WorldRegions::iterator region = regions_.find(id);
  return region == regions_.end() ? createRegion(region_x, region_y) : region->second;
}



//----[  acquireBorderedRegionFromPoint  ]-------------------------------------
WorldRegion* Map::acquireBorderedRegionFromPoint(float x, float y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  return acquireRegionAndNeighbors(id, region_x, region_y);
}




//----[  acquireBorderedRegionFromPoint  ]-------------------------------------
WorldRegion* Map::acquireBorderedRegionFromPoint(int x, int y) {
  short region_x, region_y;
  Evidyon::World::RegionID id = World::regionIDFromPoint(x, y, &region_x, &region_y);
  return acquireRegionAndNeighbors(id, region_x, region_y);
}




//----[  updateRegion  ]-------------------------------------------------------
bool Map::updateRegion(float x, float y, const WorldRegion* current, WorldRegion** updated) {
  if (current && current->containsPoint(x, y)) return true;
  *updated = getRegionFromPoint(x, y);
  return false;
}




//----[  createRegion  ]-------------------------------------------------------
WorldRegion* Map::createRegion(short region_x, short region_y) {
  World::RegionID region_id = World::regionIDFromRegionCoordinates(region_x, region_y);

  // Find all of the neighbors of this region
  short region_x_m1 = region_x-1;
  short region_x_p1 = region_x+1;
  short region_y_m1 = region_y-1;
  short region_y_p1 = region_y+1;
  WorldRegion* neighbors[] = {
    getRegion(region_x_m1, region_y_m1),
    getRegion(region_x_m1, region_y),
    getRegion(region_x_m1, region_y_p1),
    getRegion(region_x, region_y_m1),
    getRegion(region_x, region_y_p1),
    getRegion(region_x_p1, region_y_m1),
    getRegion(region_x_p1, region_y),
    getRegion(region_x_p1, region_y_p1),
  };

  GlobalWorldManager* manager = GlobalWorldManager::singleton();
  WorldRegion* region = manager->allocateRegion(this,
                                                neighbors,
                                                region_x,
                                                region_y,
                                                region_id);
  CONFIRM(region != NULL) else return NULL;
  regions_.insert(WorldRegions::value_type(region_id, region));

  if (region_x >=0 && region_y >= 0 &&
      region_x < regions_wide_ && region_y < regions_high_)
  { // active the swarm spawner in this region
    const int region_index = region_x + region_y * regions_wide_;
    RegionSwarmSpawner* swarm_spawner = &region_swarm_spawners_[region_index];
    swarm_spawner->activate(region);
    active_region_swarm_spawners_.push_back(swarm_spawner);
    
    // spawn stuff in the region
    swarm_spawner->update(Server::GlobalLifeformAIManager::singleton());
  }

  return region;
}




//----[  acquireRegionAndNeighbors  ]------------------------------------------
WorldRegion* Map::acquireRegionAndNeighbors(World::RegionID region_id,
                                            short region_x,
                                            short region_y) {
  WorldRegion* region = NULL;
  {
    WorldRegions::iterator region_iterator = regions_.find(region_id);
    region = region_iterator == regions_.end() ? createRegion(region_x, region_y) : region_iterator->second;
  }

  //World::RegionID region_id = World::regionIDFromRegionCoordinates(region_x, region_y);
  //WorldRegion* region = createRegion(region_x, region_y);

  // Add all of the neighbors
  short region_x_m1 = region_x-1;
  short region_x_p1 = region_x+1;
  short region_y_m1 = region_y-1;
  short region_y_p1 = region_y+1;
  short neighbor_coordinates[8][2] = {
    region_x_m1, region_y_m1,
    region_x_m1, region_y,
    region_x_m1, region_y_p1,
    region_x, region_y_m1,
    region_x, region_y_p1,
    region_x_p1, region_y_m1,
    region_x_p1, region_y,
    region_x_p1, region_y_p1,
  };

  for (int i = 0; i < 8; ++i) { // acquire all of this region's neighbors
    short neighbor_x = neighbor_coordinates[i][0];
    short neighbor_y = neighbor_coordinates[i][1];

    WorldRegion* neighbor_region = getRegion(neighbor_x, neighbor_y);
    if (NULL == neighbor_region) {
      neighbor_region = createRegion(neighbor_x, neighbor_y);
    }

    CONFIRM(neighbor_region != NULL);
  }


  // Return
  return region;
}




//----[  getRegion  ]----------------------------------------------------------
WorldRegion* Map::getRegion(short region_x, short region_y) {
  Evidyon::World::RegionID id = World::regionIDFromRegionCoordinates(region_x,region_y);
  WorldRegions::iterator region = regions_.find(id);
  if (region == regions_.end()) return NULL;
  return region->second;
  
}



//----[  addClearItemsOnLocation  ]--------------------------------------------
void Map::addClearItemsOnLocation(bool important, int x, int y, ItemIDType summation) {
  ClearItemsOnMapLocation clear_struct = {
      x,
      y,
      GetTickCount() + (important ? IMPORTANT_ITEM_DURATION_MILLIS : NORMAL_ITEM_DURATION_MILLIS),
      summation };
  clear_items_on_location_queue_.push(clear_struct);
}




}

