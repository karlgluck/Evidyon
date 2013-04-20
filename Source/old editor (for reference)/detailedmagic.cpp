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
#include "detailedmagic.h"
#include "../shared/actor/evidyon_actor.h"

/// This class implements the Magic interface
dc::dcClass<Evidyon::DetailedMagic>::Implements<Evidyon::Magic> Evidyon::DetailedMagic::implements_magic_;

namespace Evidyon {


#define D3DX_PI    ((float)  3.141592654f)
#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))

DetailedMagicStage::DetailedMagicStage() {
  member("Effect Type", &effect_type_);
  member("Effect Duration", &duration_);
  member("Effect Parameter:  float", &float_param_);
  member("Effect Parameter:  int", &int_param_);
  member("Effect Enchantment", &enchantment_);
  member("Targeting Type", &targeting_type_);
  member("Number of Targets", &targets_);
  member("Delay", &delay_);
  member("Spread", &spread_);
  member("AOE MagicFX", &aoe_fx_);
  member("Per-Targeter MagicFX", &per_targeter_magic_fx_);
  member("Per-Targeter ProjectileFX", &per_targeter_projectile_fx_);

  // Disable
  effect_type_.setValue(MagicDescriptionStageEffect::INVALID);
  targeting_type_.setValue(MagicDescriptionStageTargeting::SELF);
}

void DetailedMagicStage::compileForServer(MagicDescriptionStage* description) {
  description->targeting.type = targeting_type_.getValue();
  description->targeting.parameters.targets = targets_.getValue();
  description->targeting.parameters.delay = delay_.getValue();
  { // depending on the type, the spread does different things
    switch (targeting_type_.getValue()) {
      case MagicDescriptionStageTargeting::PROJECTILE:
        description->targeting.parameters.spread = D3DXToRadian(spread_.getValue());
        break;
      default:
        description->targeting.parameters.spread = spread_.getValue()*spread_.getValue();
        break;
    }
  }
  description->effect.type = effect_type_.getValue();
  description->effect.parameters.duration = duration_.getValue();
  description->effect.parameters.float_param = float_param_.getValue();
  description->effect.parameters.int_param = int_param_.getValue();

  description->aoe_magic_fx_type = aoe_fx_.getReferencedResourceIndex();
  description->per_targeter_fx_type =
    (targeting_type_.getValue() == MagicDescriptionStageTargeting::PROJECTILE) ?
        per_targeter_projectile_fx_.getReferencedResourceIndex() :
        per_targeter_magic_fx_.getReferencedResourceIndex();

  // Fill out the enchantment parameters if this is an enchantment
  Enchantment* enchantment = enchantment_.getReferencedResource();
  if (enchantment == NULL) {
    description->effect.enchantment_class_index = -1;
    description->effect.enchantment_inv_class_mask = ~0;
    description->effect.enchantment_type_bits = 0;
  } else {
    enchantment->fillMagicDescriptionStageEffect(&description->effect);
  }
}




std::string DetailedMagicStage::staticTypeName() {
  return "DetailedMagicStage";
}




DetailedMagic::DetailedMagic() {
  member("MagicFX", &magic_fx_);
  member("Casting Action", &casting_action_);
  member("Power Type", &power_type_);
  member("Freeze", &freeze_);
  member("Movement Delay", &movement_delay_);
  member("Action Delay - Essential", &essential_action_delay_);
  member("Action Delay - Balancing", &balancing_action_delay_);

  for (int i = 0; i < (int)(Evidyon::MagicDescription::NUM_STAGES); ++i) {
    char name[128];
    sprintf_s(name, sizeof(name), "Stage %i", i);
    member(name, &stages_[i]);
  }

  power_type_.setValue(MagicDescription::NO_BONUSES);
  casting_action_.setValue(ACTORACTION_CASTSPELL1);
}



void DetailedMagic::compileForServer(MagicDescription* description) {

  description->power_type = power_type_.getValue();
  description->casting.actor_action = casting_action_.getValue();
  description->casting.freeze = freeze_.getValue();
  description->casting.movement_delay = movement_delay_.getValue();
  description->casting.essential_action_delay =
      essential_action_delay_.getValue();
  description->casting.balancing_action_delay =
      balancing_action_delay_.getValue();

  for (int i = 0; i < (int)(Evidyon::MagicDescription::NUM_STAGES); ++i) {
    stages_[i].compileForServer(&description->stages[i]);
  }
  
  {
    dc::dcList<MagicFX>::Element* element = magic_fx_.getReferencedResource();
    description->magic_fx_type = element == NULL ? -1 : element->getIndex();
  }
}




std::string DetailedMagic::staticTypeName() {
  return "DetailedMagic";
}


}