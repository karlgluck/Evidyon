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
#ifndef __SHARED_ACTOR_ACTORSYNCDATA_H__
#define __SHARED_ACTOR_ACTORSYNCDATA_H__
#pragma once


#include "actorsyncflags.h"

namespace Evidyon {
namespace Actor {


#pragma pack(push, 1)
  
//----[  ActorSyncData  ]------------------------------------------------------
struct ActorSyncData {
  unsigned __int16 x_packed, y_packed;
  ActorSyncFlags flags;

  inline void packX(float x);
  inline float unpackX() const;
  inline void packY(float y);
  inline float unpackY() const;
};

#pragma pack(pop)

//----[  packX  ]--------------------------------------------------------------
void ActorSyncData::packX(float x) {
  x_packed = (unsigned __int16)(x * 32.0f);
}

//----[  packY  ]--------------------------------------------------------------
void ActorSyncData::packY(float y) {
  y_packed = (unsigned __int16)(y * 32.0f);
}

//----[  unpackX  ]------------------------------------------------------------
float ActorSyncData::unpackX() const {
  return x_packed * (1.0f / 32.0f);
}

//----[  unpackY  ]------------------------------------------------------------
float ActorSyncData::unpackY() const {
  return y_packed * (1.0f / 32.0f);
}


}
}

#endif