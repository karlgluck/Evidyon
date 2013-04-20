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
#include "lifeformai_interactivenpc.h"
#include "shared/actor/actortemplate.h"
#include "shared/actor/tools/actortemplatecompiler.h"
#include "shared/lifeformai/lifeformai.h"
#include "shared/actor/actorprofile.h"
#include <assert.h>


namespace Evidyon {
namespace LifeformAI {
namespace Tools {


dc::dcClass<LifeformAI_InteractiveNPC>::Implements<LifeformAI>
  LifeformAI_InteractiveNPC::implements_LifeformAI;






//----[  LifeformAI_InteractiveNPC  ]------------------------------------------
LifeformAI_InteractiveNPC::LifeformAI_InteractiveNPC() {
  member("Displayed Name", &name_);
  member("Actor Form", &actor_form_);
  //member("Actor Profile Specialization", &actor_profile_specialization_);
  member("NPC Type", &npc_type_);
  for (int i = 0; i < Evidyon::LifeformAI::LifeformAI_InteractiveNPC::NUMBER_OF_PHRASES; ++i) {
    char name[128];
    sprintf_s(name, "Phrase %i", i+1);
    member(name, &phrases_[i]);
  }

  //actor_profile_specialization_.setValue(Actor::SPECIALIZE_UNARMED);
}




//----[  compile  ]------------------------------------------------------------
bool LifeformAI_InteractiveNPC::compile(
    Evidyon::Treasure::Tools::TreasureCompiler* treasure_compiler,
    Evidyon::Event::Tools::EventCompiler* event_compiler,
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Evidyon::LifeformAI::LifeformAI* description) {
  Evidyon::Tools::ActorProfileElement* actor_profile;
  Evidyon::SkinnedMesh::SkinnedMeshFormIndex form;
  actor_form_.compile(&actor_profile, &form);
  if (!actor_profile ||
      form == Evidyon::SkinnedMesh::INVALID_SKINNED_MESH_FORM_INDEX) {
    return false;
  }

  // Compile the template for this lifeform
  Actor::ActorTemplate actor_template;
  actor_profile->compileDefaultTemplate(&actor_template);
  actor_template.alignment
    = Actor::ALIGNMENT_EVIL;
  actor_template.skinned_mesh_form = form;
  strncpy_s(actor_template.name,
            Actor::ActorTemplate::MAX_NAME_LENGTH,
            name_.getValue().c_str(),
            Actor::ActorTemplate::MAX_NAME_LENGTH-1);
  actor_template.template_type = Actor::STATIC_TEMPLATE;
  switch (npc_type_.getValue()) {
  case Evidyon::LifeformAI::LifeformAI_InteractiveNPC::STORAGE:
    actor_template.interaction = Actor::ACTORINTERACTION_OPEN_STORAGE;
    break;
  case Evidyon::LifeformAI::LifeformAI_InteractiveNPC::BAZAAR:
    actor_template.interaction = Actor::ACTORINTERACTION_OPEN_BAZAAR;
    break;
  case Evidyon::LifeformAI::LifeformAI_InteractiveNPC::SPOKESMAN:
    actor_template.interaction = Actor::ACTORINTERACTION_TRIGGER;
    break;
  case Evidyon::LifeformAI::LifeformAI_InteractiveNPC::DAMAGE_TARGET:
    actor_template.interaction = Actor::ACTORINTERACTION_ATTACK;
    break;
  case Evidyon::LifeformAI::LifeformAI_InteractiveNPC::HEALER:
    actor_template.interaction = Actor::ACTORINTERACTION_TRIGGER;
    break;
  }
  actor_template.alignment = Actor::ALIGNMENT_GOOD;
  actor_template.profile_specialization
    = Actor::SPECIALIZE_UNARMED;
    //= actor_profile_specialization_.getValue();
  Actor::ActorTemplateIndex actor_template_index
    = actor_template_compiler->addTemplate(&actor_template);
  assert(actor_template_index != Actor::INVALID_ACTOR_TEMPLATE_INDEX);

  // Write the local lifeform description
  description->type
    = Evidyon::LifeformAI::LifeformAI::LIFEFORMAI_INTERACTIVENPC;
  description->spawn_delay = 0;
  description->spawn_event = Event::INVALID_EVENT_INDEX;
  Evidyon::LifeformAI::LifeformAI_InteractiveNPC* my_description
    = &description->interactive_npc;
  my_description->actor_template_index = actor_template_index;
  my_description->type = npc_type_.getValue();
  for (int i = 0; i < Evidyon::LifeformAI::LifeformAI_InteractiveNPC::NUMBER_OF_PHRASES; ++i) {
    strncpy_s(my_description->phrases[i],
              Evidyon::LifeformAI::LifeformAI_InteractiveNPC::PHRASE_LENGTH,
              phrases_[i].getValue().c_str(),
              Evidyon::LifeformAI::LifeformAI_InteractiveNPC::PHRASE_LENGTH-1);
  }

  return true;
}



//----[  preview  ]------------------------------------------------------------
bool LifeformAI_InteractiveNPC::preview(
    Evidyon::Tools::ActorProfileElement** actor_profile,
    Evidyon::Tools::SkinnedMeshFormElement** form) {
  assert(actor_profile);
  assert(form);
  actor_form_.compile(actor_profile, form);
  return (*actor_profile) && (*form);
}





}
}
}

