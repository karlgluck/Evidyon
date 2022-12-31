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
#include "detailedmagic.h"


template<> const char* dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::getString(
  Evidyon::MagicDescriptionStageTargeting::Type value) {
  using namespace Evidyon;
  switch (value) {
    case MagicDescriptionStageTargeting::SELF: return "Self";
    case MagicDescriptionStageTargeting::PROJECTILE: return "Projectile";
    case MagicDescriptionStageTargeting::ANY_TARGET_ACTOR: return "Any Target Actor";
    case MagicDescriptionStageTargeting::NONCASTER_TARGET_ACTOR: return "Non-Caster Target Actor";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER: return "AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_CASTER_OFFENSIVE: return "Offensive AOE - Caster-Bound";
    case MagicDescriptionStageTargeting::AOE_ACTORBOUND_OTHER_OFFENSIVE:  return "Offensive AOE - Actor-Bound";
    case MagicDescriptionStageTargeting::AOE_LOCATIONBOUND_OFFENSIVE: return "Offensive AOE - Location-Bound";
    case MagicDescriptionStageTargeting::PERIODIC_AOE_LOCATIONBOUND_OFFENSIVE: return "Periodic Offensive AOE - Location-Bound";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::MagicDescriptionStageTargeting::Type>::staticTypeName()
{
    return "Enum-MagicDescriptionStageTargetingType";
}

