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