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
#include "avatarracelist.h"
#include "shared/avatar/avatarraceclientdescription.h"
#include "shared/avatar/avatarraceserverdescription.h"
#include <assert.h>


namespace Evidyon {
namespace Avatar {
namespace Tools {




  
//----[  AvatarRaceList  ]-----------------------------------------------------
AvatarRaceList::AvatarRaceList() {
  for (int i = 0; i < NUMBER_OF_AVATAR_RACES; ++i) {
    member(Avatar::AvatarRaceString((Avatar::AvatarRace)i), &races_[i]);
  }
}



//----[  compile  ]------------------------------------------------------------
bool AvatarRaceList::compile(
    Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
    Avatar::AvatarRaceClientDescription
      client_descriptions[Avatar::NUMBER_OF_AVATAR_RACES],
    Avatar::AvatarRaceServerDescription
      server_descriptions[Avatar::NUMBER_OF_AVATAR_RACES]) {
  for (int i = 0; i < Avatar::NUMBER_OF_AVATAR_RACES; ++i) {
    if (!races_[i].compile(actor_template_compiler,
                           &client_descriptions[i],
                           &server_descriptions[i])) {
      return false;
    }
  }

  return true;
}

void AvatarRaceList::getForm(Avatar::AvatarRace race,
                             Avatar::AvatarGender gender,
                             Evidyon::Actor::Tools::ActorFormReference* form) {
  races_[race].getForm(gender, form);
}


}
}
}

