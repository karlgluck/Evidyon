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
#ifndef __SHARED_GEOSID_GEOSIDSERVERDESCRIPTION_H__
#define __SHARED_GEOSID_GEOSIDSERVERDESCRIPTION_H__
#pragma once

#include "shared/geosid/geonite.h"
#include "shared/geosid/geosidhashid.h"
#include "shared/geosid/geosidindex.h" 
#include "shared/world/mapindex.h"
#include "shared/magic/magicindex.h"
#include "common/number.h"
#include "shared/item/itemindex.h"
#include "shared/event/eventindex.h"
#include "shared/time/milliseconds.h"


namespace Evidyon {
namespace Geosid {


//----[  GeosidServerDescription  ]--------------------------------------------
struct GeosidServerDescription {

  // This geosid's index on the client
  GeosidIndex index;
  
  // Permanent storage hash ID number.
  GeosidHashID id;
  
  // if true:
  //  - geosid cannot be owned
  //  - items can be sacrificed here
  //  - anyone can use burst abilities
  // if false:
  //  - geosid is "wild" and can be owned by guild
  //  - items cannot be sacrificed here
  //  - only owners can use burst abilities
  bool is_white_geosid;

  // What this geosid does for owners (ignored if white)
  float hp_bonus, mp_bonus, xp_bonus;
  
  // This ability is triggered when the caster uses "burst" near the geosid.
  // the source location is the geosid, source object is the caster, target
  // object and target location are specified by the target
  Magic::MagicIndex burst_magic;
  Geosid::Geonite burst_cost;

  // The coordinates of this geosid
  World::MapIndex map;
  NumberNoConstructor x, z;

  Geosid::Geonite geonite_to_activate;
  Event::EventIndex activation_event;
  Time::Milliseconds activation_event_duration; // used for portal length

  // one of these items is randomly generated when the geosid is activated
  static const int MAX_ITEMS = 8;
  Item::ItemIndex items[MAX_ITEMS];
  size_t number_of_items;

  static const int MAX_PORTALS = 10;
  struct {
    World::MapIndex map;
    int x, z;
  } portals[MAX_PORTALS];
  size_t number_of_portals;

  // The amount of geonite needed for each activation of this geosid's special
  // ability.
  //Geosid::Geonite geonite_to_activate;
  //static const int NUMBER_OF_GEONITE_ABILITIES = 8;
  //enum Type {
  //  GENERATE_ITEM,
  //  GENERATE_PORTAL,
  //} type;
  //union {
  //  Evidyon::Item::ItemIndex items
  //  struct Portal {
  //    World::MapIndex map;
  //    int x, z;
  //    Event::EventIndex event_index;
  //  } portal[NUMBER_OF_GEONITE_ABILITIES];
  //};
};




}
}

#endif