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
#ifndef __EVIDYON_ACTOR_H__
#define __EVIDYON_ACTOR_H__
#pragma once

#include "shared/actor/actoralignment.h"
#include "shared/actor/actorsound.h"
//#include "shared/actor/actoraction.h"

namespace Evidyon {



typedef unsigned int ActorTypeIndex;

enum ActorAttachmentPoint {
  ACTORATTACHMENTPOINT_HELMET,
  ACTORATTACHMENTPOINT_WEAPON,
  ACTORATTACHMENTPOINT_SHIELD,

  ACTORATTACHMENTPOINT_COUNT,
  ACTORATTACHMENTPOINT_INVALID,
};



enum ActorAttack {
  ACTORATTACK_1,
  ACTORATTACK_2,
  ACTORATTACK_3,
  ACTORATTACK_4,

  ACTORATTACK_COUNT,
};

//
///**
// * Different things an actor does
// */
//enum ActorAction
//{
//  ACTORACTION_ATTACK1, // uses actor sound 1
//  ACTORACTION_ATTACK2, // uses sound 1
//  ACTORACTION_ATTACK3, // uses sound 2
//  ACTORACTION_ATTACK4, // uses sound 2
//  ACTORACTION_BLOCK1,
//  ACTORACTION_BLOCK2,
//  ACTORACTION_BLOCK3,
//  ACTORACTION_CASTSPELL1,
//  ACTORACTION_CASTSPELL2,
//  ACTORACTION_GOTHIT,
//  ACTORACTION_DIE,
//
//  ACTORACTION_NONE,
//
//  ACTORACTION_INVALID,
//
//  ACTORACTION_COUNT = ACTORACTION_NONE,
//
//  // Actions whose speed varies depending on the attributes of the character.
//  // The speed values are used to modify how quickly the animation plays.
//  ACTORACTION_VARIABLE_SPEED_COUNT = ACTORACTION_GOTHIT,
//};
//
//#if (ACTORACTION_COUNT >= (1<<4))
//#error Too many actor actions to fit into an actor sync packet!
//#endif

}

#endif