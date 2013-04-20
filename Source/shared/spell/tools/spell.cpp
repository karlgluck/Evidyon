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
#include "spell.h"
#include "shared/spell/spellclientdescription.h"
#include "shared/spell/spellserverdescription.h"
#include "shared/avatar/spellavailabilitymask.h"



namespace Evidyon {
namespace Spell {
namespace Tools {

  
//----[  Spell  ]--------------------------------------------------------------
Spell::Spell() {
  member("Displayed Name", &name_);
  member("Description", &description_);
  member("MP Cost", &mp_cost_);
  member("Casting Requires Target", &casting_requires_target_);
  member("Casting Requires Slot", &casting_requires_slot_);
  member("Can Target Caster", &can_target_caster_);
  member("Can Target Dead", &can_target_dead_);
  member("Casting Action", &casting_action_);
  member("Stages", &stages_);


  mp_cost_.setValue(1);
  casting_requires_target_.setValue(true);
  casting_action_.setValue(Actor::ACTORACTION_CAST_TARGETED_SPELL);
}


//----[  compile  ]------------------------------------------------------------
void Spell::compile(Magic::Tools::MagicCompiler* magic_compiler,
                    SpellClientDescription* client_description,
                    SpellServerDescription* server_description) {

  // Fill the client description
  strncpy_s(client_description->name,
            SpellClientDescription::NAME_LENGTH,
            name_.getValue().c_str(),
            SpellClientDescription::NAME_LENGTH - 1);
  strncpy_s(client_description->description,
            SpellClientDescription::DESCRIPTION_LENGTH,
            description_.getValue().c_str(),
            SpellClientDescription::DESCRIPTION_LENGTH - 1);
  client_description->needs_target = casting_requires_target_.getValue();
  client_description->needs_slot = casting_requires_slot_.getValue();
  client_description->can_target_caster = can_target_caster_.getValue();
  client_description->can_target_dead = can_target_dead_.getValue();

  // Fill the server description
  server_description->mp_cost = mp_cost_.getValue();
  server_description->casting_action = casting_action_.getValue();
  server_description->number_of_stages = stages_.getMembers().size();

  const dc::dcGenericResource::Array& stages = stages_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  size_t index = 0;
  for (i = stages.begin(); i != stages.end(); ++i, ++index) {
    Evidyon::Tools::SpellStageElement* stage
      = (Evidyon::Tools::SpellStageElement*)(*i);
    stage->compile(magic_compiler, &server_description->stages[index]);
  }
}



//----[  implement  ]----------------------------------------------------------
void Spell::implement(const std::string& name,
                      unsigned long type) {
  name_.setValue(name);
  //magic_.implementByType(type);
  // disabled
}

//----[  set  ]----------------------------------------------------------------
void Spell::set(std::map<std::string,std::string>* values) {
  const std::string& stage_name = values->find("Stage Number")->second;
  SpellStage* stage = (SpellStage*)stages_.getMemberByName(stage_name);
  if (!stage) stage = (SpellStage*)stages_.generateElement(stage_name);
  stage->set(values);

  name_.setValue(values->find("Name")->second);
  description_.setValue(values->find("Description")->second);

  mp_cost_.interpret(values->find("MP Cost")->second.c_str());
  casting_requires_target_.interpret(
    values->find("Casting Requires Target")->second.c_str());
  can_target_caster_.interpret(
    values->find("Can Target Caster")->second.c_str());
  can_target_dead_.interpret(
    values->find("Can Target Dead")->second.c_str());
}



//----[  getLevelAcquired  ]---------------------------------------------------
bool Spell::getLevelAcquired(Avatar::AvatarClass avatar_class,
                             Avatar::AvatarLevel* level) {
  bool found = false;

  const dc::dcGenericResource::Array& stages = stages_.getMembers();
  dc::dcGenericResource::Array::const_iterator i;
  size_t index = 0;
  for (i = stages.begin(); i != stages.end(); ++i, ++index) {
    Evidyon::Tools::SpellStageElement* stage
      = (Evidyon::Tools::SpellStageElement*)(*i);
    Avatar::AvatarLevel acquired_level;
    if (stage->getLevelAcquired(avatar_class, &acquired_level)) {
      if (found) {
        if (acquired_level < *level) {
          *level = acquired_level;
        }
      } else {
        *level = acquired_level;
        found = true;
      }
    }
  }

  return found;
}





//----[  addToClassSpellAvailabilityMaskAtAcquiredLevel  ]---------------------
void Spell::addToClassSpellAvailabilityMaskAtAcquiredLevel(
    Avatar::AvatarClass avatar_class,
    Avatar::SpellAvailabilityMask* level_masks,
    size_t number_of_level_masks) {

  Avatar::AvatarLevel level_acquired = -1;
  if (getLevelAcquired(avatar_class, &level_acquired)) {
    --level_acquired;
    if (level_acquired < 0) level_acquired = 0;
    if (level_acquired < number_of_level_masks) {
      level_masks[level_acquired].setBit(getIndex());
    }
  }
}



}
}
}


