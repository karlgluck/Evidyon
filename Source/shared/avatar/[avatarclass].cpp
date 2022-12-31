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

