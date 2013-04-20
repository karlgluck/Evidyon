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
#ifndef __SHARED_ACTOR_ACTORFASTSYNC_H__
#define __SHARED_ACTOR_ACTORFASTSYNC_H__
#pragma once

#include "actorsyncdata.h"

namespace Evidyon {
namespace Actor {


static const double ACTOR_FAST_SYNC_PERIOD = 1.0 / 10.0;


#pragma pack(push, 1)


  
//----[  ActorFastSync  ]------------------------------------------------------
struct ActorFastSync {
  // This flag is 1 if stat_value is HP.  For 1, the client should reset
  // a counter that is incremented each time a fast sync is received.
  unsigned stat_type_reset : 1;
  // Depending on the value of the client's counter, this value is:
  //  0, 2 - %hp  * (2^7-1)
  //  1, 3 - %mp  * (2^7-1)
  //  4    - %exp * (2^7-1)
  unsigned stat_value : 7;
  float time_since_last_sync;

  ActorSyncData data;
};

#pragma pack(pop)

}
}

#endif