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
#ifndef __FASTSYNC_H__
#define __FASTSYNC_H__
#pragma once



#include "../networkdecl.h"
#include "../actor/actorsync.h"

namespace Evidyon {
namespace Network {
namespace Packet {

#pragma pack( push, 1 )

struct FastSyncPacket {
  // This flag is 1 if stat_value is HP.  For 1, the client should reset
  // a counter that is incremented each time a fast sync is received.
  unsigned stat_type_reset : 1;
  // Depending on the value of the client's counter, this value is:
  //  0, 2 - %hp  * (2^7-1)
  //  1, 3 - %mp  * (2^7-1)
  //  4    - %exp * (2^7-1)
  unsigned stat_value : 7;
  float time_since_last_sync;
  unsigned __int16 x_packed, y_packed;
  ActorSyncData::Flags sync_flags;
};

#pragma pack( pop )

}
}
}



#endif