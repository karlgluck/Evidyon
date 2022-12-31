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

