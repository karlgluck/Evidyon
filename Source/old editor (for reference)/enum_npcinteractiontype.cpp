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
#include "../shared/server-editor/lifeformaidescription.h"
#include <dc/enum>



template<>
const char* dc::dcEnum<Evidyon::NPCInteractionType>::getString(Evidyon::NPCInteractionType value) {
  using namespace Evidyon;
  switch (value) {
    case NPCINTERACTION_STORAGE:    return "Storage";
    case NPCINTERACTION_BAZAAR:     return "Bazaar";
    case NPCINTERACTION_GEOSID:     return "Geosid";
    case NPCINTERACTION_QUEST:      return "Quest";
    case NPCINTERACTION_MERCHANT:   return "Merchant";
    default: return 0;
  }
}

std::string dc::dcEnum<Evidyon::NPCInteractionType>::staticTypeName()
{
    return "Enum-NPCInteractionType";
}
