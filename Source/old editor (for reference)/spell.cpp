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