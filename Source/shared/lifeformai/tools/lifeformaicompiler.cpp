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
#include "lifeformaicompiler.h"
#include "shared/lifeformai/lifeformai.h"


namespace Evidyon {
namespace LifeformAI {
namespace Tools {



  
//----[  LifeformAICompiler  ]-------------------------------------------------
LifeformAICompiler::LifeformAICompiler(
    Treasure::Tools::TreasureCompiler* treasure_compiler,
    Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler) {
  treasure_compiler_ = treasure_compiler;
  event_compiler_ = event_compiler;
  actor_template_compiler_ = actor_template_compiler;
}

  
//----[  add  ]----------------------------------------------------------------
LifeformAIIndex LifeformAICompiler::add(const LifeformAI& description) {
  return (LifeformAIIndex)lifeforms_.add(description);
}

//#define IMPLEMENT_ADD(Struct, id, member)\
//  LifeformAIIndex LifeformAICompiler::add(const Struct* description) {\
//    LifeformAI lifeform;\
//    lifeform.type = LifeformAI::id;\
//    memcpy(&lifeform.member, description, sizeof(*description));\
//    return add(lifeform);\
//  }
//
//
//IMPLEMENT_ADD(LifeformAI_GeosidGuardian, LIFEFORMAI_GEOSIDGUARDIAN, geosid_guardian)
//IMPLEMENT_ADD(LifeformAI_SimpleMeleeMonster, LIFEFORMAI_SIMPLEMELEEMONSTER, simple_melee_monster)
//IMPLEMENT_ADD(LifeformAI_SimpleMeleeTownGuard, LIFEFORMAI_SIMPLEMELEETOWNGUARD, simple_melee_town_guard)
//IMPLEMENT_ADD(LifeformAI_InteractiveNPC, LIFEFORMAI_INTERACTIVENPC, interactive_npc)
//IMPLEMENT_ADD(LifeformAI_DirectDamageMonster, LIFEFORMAI_DIRECTDAMAGEMONSTER, direct_damage_monster)
//IMPLEMENT_ADD(LifeformAI_ProjectileMonster, LIFEFORMAI_PROJECTILEMONSTER, projectile_monster)
//
//#undef IMPLEMENT_ADD

//----[  numberOfLifeforms  ]--------------------------------------------------
size_t LifeformAICompiler::numberOfLifeforms() {
  return lifeforms_.objects.size();
}



//----[  compile  ]------------------------------------------------------------
void LifeformAICompiler::compile(size_t index, LifeformAI* description) {
  lifeforms_.copyOut(index, description);
}


Treasure::Tools::TreasureCompiler* LifeformAICompiler::treasureCompiler() { return treasure_compiler_; }
Event::Tools::EventCompiler* LifeformAICompiler::eventCompiler() { return event_compiler_; }
Actor::Tools::ActorTemplateCompiler* LifeformAICompiler::actorTemplateCompiler() { return actor_template_compiler_; }



}
}
}

