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
#ifndef __ACTORACTION_H__
#define __ACTORACTION_H__
#pragma once



namespace Evidyon {
namespace Actor {



//----[  ActorAction  ]--------------------------------------------------------
// These are in priority order; if an action comes along while another is
// being executed, the animation for that action is only applied if it has
// a higher priority than the current one.
enum ActorAction {
  ACTORACTION_DIE,
  ACTORACTION_SPIN_ATTACK,
  ACTORACTION_CAST_AREA_SPELL,
  ACTORACTION_CAST_TARGETED_SPELL,
  ACTORACTION_ATTACK,
  ACTORACTION_GOT_HIT,
  ACTORACTION_LEAP,
  NUMBER_OF_ACTOR_ACTIONS,
  ACTORACTION_NONE = NUMBER_OF_ACTOR_ACTIONS,
};




//----[  ActorActionString  ]--------------------------------------------------
const char* ActorActionString(ActorAction value);




}
}

#endif