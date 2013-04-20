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
#ifndef __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAI_INTERACTIVENPC_H__
#define __SHARED_LIFEFORMAI_TOOLS_LIFEFORMAI_INTERACTIVENPC_H__
#pragma once

#include "shared/actor/tools/actorformreference.h"
#include "shared/actor/actortemplateindex.h"
#include "shared/scenery/tools/scenery.h"
#include "shared/actor/actorequipmentslot.h"
#include "lifeformai.h"
#include "shared/lifeformai/lifeformai_interactivenpc.h"
#include <dc/string>
#include <dc/enum>

namespace Evidyon {
namespace LifeformAI {
namespace Tools {
  
//----[  LifeformAI_InteractiveNPC  ]------------------------------------------
class LifeformAI_InteractiveNPC
  : public dc::dcResource<LifeformAI_InteractiveNPC, LifeformAI> {
public:
  static dc::dcClass<LifeformAI_InteractiveNPC>::Implements<LifeformAI> implements_LifeformAI;
  static std::string staticTypeName() { return "LifeformAI_InteractiveNPC"; }

public:
  LifeformAI_InteractiveNPC();
  virtual bool compile(
    Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
    Evidyon::Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Evidyon::LifeformAI::LifeformAI* description);
  virtual bool preview(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::Tools::SkinnedMeshFormElement** form);

  Evidyon::Tools::ActorProfileReference* getActorProfile() {
    return actor_form_.getActorProfile();
  }

  Evidyon::Tools::SkinnedMeshFormReference* getSkinnedMeshForm() {
    return actor_form_.getForm();
  }

private:
  dc::dcString name_;
  Evidyon::Actor::Tools::ActorFormReference actor_form_;
  Evidyon::Tools::SceneryReference equipment_[Actor::NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
  dc::dcEnum<Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type> npc_type_;
  dc::dcString phrases_[Evidyon::LifeformAI::LifeformAI_InteractiveNPC::NUMBER_OF_PHRASES];
};


}
}
}

#endif