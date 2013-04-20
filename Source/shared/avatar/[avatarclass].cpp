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
#include "avatarclass.h"
#include "common/null.h"

// there is a problem with linking: vc++ will consider two files with the same
// name to be the same, even if they are in different folders!  so this file
// produces a linker error if it is named without [].


namespace Evidyon {
namespace Avatar {


//----[  AvatarClassString  ]--------------------------------------------------
const char* AvatarClassString(AvatarClass value) {
  switch (value) {
  default: return NULL;
  case AVATARCLASS_ADEPT: return "Adept";
  case AVATARCLASS_SORCERER: return "Sorcerer";
  case AVATARCLASS_HUNTER: return "Hunter";
  case AVATARCLASS_WARRIOR: return "Warrior";
  }
}



//----[  AvatarClassIsMage  ]--------------------------------------------------
bool AvatarClassIsMage(AvatarClass value) {
  switch (value) {
    default: return false;
    case AVATARCLASS_ADEPT:
    case AVATARCLASS_SORCERER: return true;
    case AVATARCLASS_HUNTER:
    case AVATARCLASS_WARRIOR: return false;
  }
}


}
}

