//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
//                                                                           //
//  Karl Gluck can be reached by email at kwg8@cornell.edu                   //
//---------------------------------------------------------------------------//
#ifndef __SERVER_PLAYER_PLAYERTRAPINSTANCE_H__
#define __SERVER_PLAYER_PLAYERTRAPINSTANCE_H__
#pragma once


#include "shared/time/timer.h"
#include "shared/event/eventid.h"

namespace Evidyon {
class WorldRegion;
}

namespace Evidyon {
namespace Server {


//----[  PlayerTrapInstance  ]-------------------------------------------------
struct PlayerTrapInstance {
  enum {
    DIMENSION_DOOR,
    BLACK_HOLE,
    INVALID,
  } type;

  union {
    struct {
      Event::EventID event_id;
      float radius;
    } black_hole;
    struct {
      WorldRegion* source_region;
      float target_x, target_z;
      float source_x, source_z;
    } dimension_door;
  };

  Time::Timer<Time::RESOLUTION_100_MS> expiration;

  void zero();
};


}
}


#endif