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
#include <dc/enum>
#include "shared/lifeformai/lifeformai_interactivenpc.h"
#include "common/null.h"



//----[  getString  ]----------------------------------------------------------
template<>
const char* dc::dcEnum<Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type>
    ::getString(Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type value) {
  using namespace Evidyon::LifeformAI;
  switch (value) {
  case LifeformAI_InteractiveNPC::STORAGE: return "Storage";
  case LifeformAI_InteractiveNPC::BAZAAR: return "Bazaar";
  case LifeformAI_InteractiveNPC::SPOKESMAN:  return "Spokesman";
  case LifeformAI_InteractiveNPC::DAMAGE_TARGET:  return "Damage Target";
  case LifeformAI_InteractiveNPC::HEALER: return "Healer";
    default:                                return NULL;
  }
}



//----[  staticTypeName  ]-----------------------------------------------------
std::string dc::dcEnum<Evidyon::LifeformAI::LifeformAI_InteractiveNPC::Type>::staticTypeName()
{
    return "Enum-LifeformAI-LifeformAI_InteractiveNPC-Type";
}
