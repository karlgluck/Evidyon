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
#include "lifeformaiinstance.h"
#include "shared/lifeformai/lifeformai.h"
#include "lifeformaiinstance_simplemeleemonster.h"
#include "lifeformaiinstance_interactivenpc.h"
#include "lifeformaiinstance_simplemeleetownguard.h"
#include "lifeformaiinstance_geosidguardian.h"
#include "lifeformaiinstance_directdamagemonster.h"
#include "common/null.h"
#include "common/safemacros.h"
#include <assert.h>

namespace Evidyon {
namespace Server {




  
//----[  New  ]----------------------------------------------------------------
LifeformAIInstance* LifeformAIInstance::New(
    Avatar::AvatarLevel intended_level,
    Number x,
    Number z,
    const LifeformAI::LifeformAI* description) {
  if (!description) return NULL;
  switch (description->type) {
  default: assert(false); return NULL;
  case LifeformAI::LifeformAI::LIFEFORMAI_SIMPLEMELEEMONSTER: {
    LifeformAIInstance_SimpleMeleeMonster* instance
      = new LifeformAIInstance_SimpleMeleeMonster(intended_level, x, z, &description->simple_melee_monster);
    return instance;
    } break;
  case LifeformAI::LifeformAI::LIFEFORMAI_INTERACTIVENPC: {
    LifeformAIInstance_InteractiveNPC* instance
      = new LifeformAIInstance_InteractiveNPC(x, z, &description->interactive_npc);
    return instance;
    } break;
  case LifeformAI::LifeformAI::LIFEFORMAI_SIMPLEMELEETOWNGUARD: {
    LifeformAIInstance_SimpleMeleeTownGuard* instance
      = new LifeformAIInstance_SimpleMeleeTownGuard(x, z, &description->simple_melee_town_guard);
    return instance;
    } break;
  case LifeformAI::LifeformAI::LIFEFORMAI_GEOSIDGUARDIAN: {
    LifeformAIInstance_GeosidGuardian* instance
      = new LifeformAIInstance_GeosidGuardian(x, z, &description->geosid_guardian);
    return instance;
    } break;
  //case LifeformAI::LifeformAI::LIFEFORMAI_PROJECTILEMONSTER: {
  //  LifeformAIInstance_ProjectileMonster* instance
  //    = new LifeformAIInstance_ProjectileMonster(intended_level, x, z, &description->projectile_monster);
  //  return instance;
  //  } break;
  case LifeformAI::LifeformAI::LIFEFORMAI_DIRECTDAMAGEMONSTER: {
    LifeformAIInstance_DirectDamageMonster* instance
      = new LifeformAIInstance_DirectDamageMonster(intended_level, x, z, &description->direct_damage_monster);
    return instance;
    } break;
  }
}


}
}

