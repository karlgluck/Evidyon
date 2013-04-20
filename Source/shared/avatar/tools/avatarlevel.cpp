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
#include "avatarlevel.h"
#include "shared/avatar/avatarleveldescription.h"


namespace Evidyon {
namespace Avatar {
namespace Tools {



  
//----[  AvatarLevel  ]--------------------------------------------------------
AvatarLevel::AvatarLevel() {
  // note: name is LEGACY, change once I have the time to reenter the values
  // TODO
  member("Exp. to Next Level", &experience_to_achieve_next_level_);
}


//----[  compile  ]------------------------------------------------------------
void AvatarLevel::compile(AvatarLevelDescription* level_description) {
  level_description->experience_to_achieve_next_level
    = experience_to_achieve_next_level_.getValue();

  // this value has to be set by the level list which is compiling this object
  level_description->total_experience_to_achieve_this_level = 0;
}




}
}
}

