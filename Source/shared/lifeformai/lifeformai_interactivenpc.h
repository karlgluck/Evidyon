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