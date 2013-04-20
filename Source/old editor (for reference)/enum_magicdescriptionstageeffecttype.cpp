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



template<> const char* dc::dcEnum<Evidyon::MagicDescriptionStageEffect::Type>::getString(
    Evidyon::MagicDescriptionStageEffect::Type value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescriptionStageEffect::INVALID:  return "Invalid";
    case MagicDescriptionStageEffect::WEAPON:   return "Weapon";
    case MagicDescriptionStageEffect::HP:       return "HP";
    case MagicDescriptionStageEffect::HP_NOARMOR: return "HP (No Armor)";
    case MagicDescriptionStageEffect::MP:       return "MP";
    case MagicDescriptionStageEffect::TELEPORT: return "Teleport";
    case MagicDescriptionStageEffect::TELEPORT_SWAP:  return "Teleport - Swap";
    case MagicDescriptionStageEffect::DISENCHANT:     return "Disenchant";
    case MagicDescriptionStageEffect::ENCHANT_HP:     return "Enchant HP";
    case MagicDescriptionStageEffect::ENCHANT_MP:     return "Enchant MP";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE: return "Mutate to Actor Type";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_GASFORM:   return "Mutate to Gasform";
    case MagicDescriptionStageEffect::ENCHANT_MUTATE_COPY:      return "Mutate to Copy of Target";
    case MagicDescriptionStageEffect::ENCHANT_ATTACK_RUN:       return "Attack Run";
    case MagicDescriptionStageEffect::ENCHANT_MAXHP:            return "Enchant Max HP";
    case MagicDescriptionStageEffect::ENCHANT_MAXMP:            return "Enchant Max MP";
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DAMAGE:  return "Enchant Physical Damage";
    case MagicDescriptionStageEffect::ENCHANT_PHYSICAL_DEFENSE: return "Enchant Physical Defense";
    case MagicDescriptionStageEffect::ENCHANT_MAGICAL_DEFENSE:  return "Enchant Magical Defense";
    case MagicDescriptionStageEffect::ENCHANT_MOVEMENT_SPEED:   return "Enchant Movement Speed";
    case MagicDescriptionStageEffect::ENCHANT_ATTACK_SPEED:     return "Enchant Attack Speed";
    case MagicDescriptionStageEffect::GEOSID_RECALL:            return "Geosid: Recall";
    case MagicDescriptionStageEffect::GEOSID_RESTOREHPMP:       return "Geosid: Restore HP/MP";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescriptionStageEffect::Type>::staticTypeName()
{
    return "Enum-MagicDescriptionStageEffectType";
}
