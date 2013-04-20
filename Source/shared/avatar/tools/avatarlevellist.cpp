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
#include "avatarlevellist.h"
#include "shared/avatar/avatarleveldescription.h"


namespace Evidyon {
namespace Avatar {
namespace Tools {



  
//----[  AvatarLevelList  ]----------------------------------------------------
AvatarLevelList::AvatarLevelList() {
  for (int i = 0; i < HIGHEST_AVATAR_LEVEL; ++i) {
    char name[64];
    _itoa_s(i+1, name, sizeof(name), 10);
    member(name, &levels_[i]);
  }
}


//----[  compile  ]------------------------------------------------------------
void AvatarLevelList::compile(
    AvatarLevelDescription avatar_levels[HIGHEST_AVATAR_LEVEL]) {
  for (int i = 0; i < HIGHEST_AVATAR_LEVEL; ++i) {
    levels_[i].compile(&avatar_levels[i]);
    avatar_levels[i].total_experience_to_achieve_this_level
      = (i == 0)
        ? 0
        : (avatar_levels[i-1].total_experience_to_achieve_this_level + 
           avatar_levels[i-1].experience_to_achieve_next_level);
  }
}



}
}
}

