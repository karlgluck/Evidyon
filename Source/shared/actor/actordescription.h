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
#ifndef __SHARED_ACTOR_ACTORDESCRIPTION_H__
#define __SHARED_ACTOR_ACTORDESCRIPTION_H__
#pragma once


#include "actorid.h"
#include "actorsyncdata.h"
#include "actorequipmentslot.h"

namespace Evidyon {
namespace Actor {


//----[  ActorDescription  ]---------------------------------------------------
#pragma pack(push, 1)
struct ActorDescription {
  static const int PROFILE_BITS = 11; // 2048 max profiles
  static const int FORM_BITS = 7;     // 128 max forms per profile

  ActorID id;
  ActorSyncData sync;
  unsigned profile_index : 11;
  unsigned form : 7;
  unsigned equipment_based_profile_specialization : 3;
  unsigned alignment : 3;
  unsigned enchantments : 16;
  unsigned __int16
    equipment_scenery[NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
};
#pragma pack(pop)


static const int MAX_ACTOR_PROFILES = 1<<ActorDescription::PROFILE_BITS;
static const int MAX_SKINNED_MESH_FORMS = 1<<ActorDescription::FORM_BITS;
static const int MAX_SCENERY = 1<<(8*sizeof(__int16));




}
}





#include "actorsync.h"
#include "../actor/evidyon_actor.h"

namespace Evidyon {

  
#pragma pack( push, 1 )

struct ActorStateDescriptionData {
  unsigned actor_type      : 12;
  unsigned corporeal_state :  3;
  unsigned enchantments    : 16;
  unsigned short equipped_items[ACTORATTACHMENTPOINT_COUNT];

  // 0: guardian
  // 1: champion
  // 2: good
  // 3: neutral
  // 4: evil
  // 5: destroyer
  // 6: dreadlord
  // 7: ***admin***
  unsigned alignment : 3;
};

#pragma pack( pop )

namespace Network {
namespace Packet {

#pragma pack( push, 1 )

// this structure is followed by the actor's name string in a full description
struct ActorDescriptionData {
  ActorSyncData sync_;
  ActorStateDescriptionData state_;
};

#pragma pack( pop )

}
}
}


#endif