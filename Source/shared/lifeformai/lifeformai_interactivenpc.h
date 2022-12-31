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
#ifndef __SHARED_LIFEFORMAI_LIFEFORMAI_INTERACTIVENPC_H__
#define __SHARED_LIFEFORMAI_LIFEFORMAI_INTERACTIVENPC_H__
#pragma once


#include "shared/actor/actortemplateindex.h"

namespace Evidyon {
namespace LifeformAI {


//----[  LifeformAI_InteractiveNPC  ]------------------------------------------
struct LifeformAI_InteractiveNPC {
  enum Type {
    STORAGE,        // provides access to an avatar's storage
    BAZAAR,         // provides access to the bazaar
    SPOKESMAN,      // speaks one of the random phrases; used like a sign
    DAMAGE_TARGET,  // speaks the amount of damage done to it each hit
    HEALER,         // restores the player to 100% HP/MP
  } type;

  Actor::ActorTemplateIndex actor_template_index;

  // Any %s in the phrases are interpreted by the client to mean the target
  // actor's name.
  static const size_t NUMBER_OF_PHRASES = 3;
  static const size_t PHRASE_LENGTH = 64;
  char phrases[NUMBER_OF_PHRASES][PHRASE_LENGTH];
};

}
}

#endif