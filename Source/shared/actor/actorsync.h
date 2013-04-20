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
#ifndef __ACTORSYNC_H__
#define __ACTORSYNC_H__
#pragma once


#include "../actor/actorid.h"

namespace Evidyon {

static const int ACTORSYNC_ANGLE_BITS = 7;

namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct ActorSyncData {
  Evidyon::Actor::ActorID id;
  unsigned __int16 x_packed, y_packed;
  struct Flags {
    unsigned moving     : 1;    // boolean
    unsigned teleport   : 1;    // 0- walk/swim/run/whatev.  1- jump instantly
    unsigned mesh_state : 2;    // 0- dead, 1- normal, 2- combat, 3- dont render mesh
    unsigned action     : 4;    // 0- nothing
    unsigned blood      : 1;    // boolean; 1 if this actor was hit by a weapon
    unsigned angle      : 7;    // if moving=0, turn to pi/380/(1<<ACTORSYNC_ANGLE_BITS)*angle
  } flags;
};

#pragma pack( pop )

}
}
}



#endif