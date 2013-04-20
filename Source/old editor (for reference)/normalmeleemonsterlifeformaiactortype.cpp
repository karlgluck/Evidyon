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
#include "normalmeleemonsterlifeformaiactortype.h"
#include "../shared/server-editor/lifeformaidescription.h"
#include "../shared/actor/actordescriptiontemplate.h"



NormalMeleeMonsterLifeformAIActorType::NormalMeleeMonsterLifeformAIActorType() {
  template_index_ = -1;

  member("Actor Type", &actor_type_);
  member("Enchantments", &enchantments_);
}


bool NormalMeleeMonsterLifeformAIActorType::compileForServer(Evidyon::AIMonsterActorType* description) {
  CONFIRM(template_index_ >= 0) else return false; // ensure that the client-side was compiled first
  description->template_index = template_index_;

  const dc::dcList<ActorType>::Element* actor_type = actor_type_.getReferencedResource();
  CONFIRM(actor_type) else return false;
  description->size = actor_type->getRadialSizeValue();

  return true;
}


bool NormalMeleeMonsterLifeformAIActorType::compileActorTemplate(int template_index,
                                                                 Evidyon::ClientActorDescriptionTemplate* client_description,
                                                                 Evidyon::ServerActorDescriptionTemplate* server_description) {
  template_index_ = template_index;

  const dc::dcList<ActorType>::Element* actor_type = actor_type_.getReferencedResource();
  CONFIRM(actor_type) else return false;
  client_description->actor_type = actor_type->getIndex();
  server_description->actor_type = actor_type->getIndex();
  server_description->radial_size = actor_type->getRadialSizeValue();

  client_description->template_type = Evidyon::ACTORTEMPLATETYPE_MONSTER;

  client_description->enchantments = 0;
  const dc::dcGenericResource::Array& enchantments = enchantments_.getMembers();
  for (dc::dcGenericResource::Array::const_iterator i = enchantments.begin();
       i != enchantments.end(); ++i) {
    dc::dcReference<Enchantment>* enchantment_reference = 
      (dcList<dcReference<Enchantment>>::Element*)(*i);
    Enchantment* enchantment = enchantment_reference->getReferencedResource();

    client_description->enchantments &= enchantment->getInvClassMask();
    client_description->enchantments |= enchantment->getTypeBits();
  }

  for (int i = 0; i < Evidyon::ACTORATTACHMENTPOINT_COUNT; ++i) {
    client_description->equipped_item_scenery[i] = 0xFFFFFFFF;
  }

  return true;
}


std::string NormalMeleeMonsterLifeformAIActorType::staticTypeName() {
  return "NormalMeleeMonsterLifeformAIActorType";
}
