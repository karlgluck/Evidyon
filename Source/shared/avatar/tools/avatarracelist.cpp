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

