//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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