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
#include "spell.h"
#include "../shared/server-editor/serverspelldescription.h"
#include "../shared/client-editor/clientspelldescription.h"

namespace Evidyon {


Spell::Spell() {
  member("Description", &description_);
  member("MP Cost", &mp_cost_);
  member("Magic", &magic_);
  member("Casting Needs Target", &casting_needs_target_);
}


void Spell::compileForClient(ClientSpellDescription* description) const {
  sprintf_s(description->name, ClientSpellDescription::MAX_NAME_LENGTH,
            "%s", getName().c_str());
  sprintf_s(description->description, ClientSpellDescription::MAX_DESCRIPTION_LENGTH,
            "%s", description_.getValue().c_str());
  description->mp_cost = mp_cost_.getValue();
  description->needs_target = casting_needs_target_.getValue();
}


void Spell::compileForServer(ServerSpellDescription* description) const {
  description->mp_cost = mp_cost_.getValue();
  description->magic_index = magic_.getReferencedResourceIndex();
}


bool Spell::castingNeedsTarget() const {
  return casting_needs_target_.getValue();
}


std::string Spell::staticTypeName() {
  return "Spell";
}



}