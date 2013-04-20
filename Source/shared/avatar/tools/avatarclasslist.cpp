//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#include "avatarclasslist.h"
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarclassclientdescription.h"
#include "shared/avatar/avatarclassserverdescription.h"
#include <assert.h>


namespace Evidyon {
namespace Avatar {
namespace Tools {



  
//----[  AvatarClassList  ]----------------------------------------------------
AvatarClassList::AvatarClassList() {
  for (int i = 0; i < NUMBER_OF_AVATAR_CLASSES; ++i) {
    member(AvatarClassString(Evidyon::Avatar::AvatarClass(i)), &classes_[i]);
  }
}




//----[  compile  ]------------------------------------------------------------
void AvatarClassList::compile(
    Avatar::AvatarClassClientDescription
      client_descriptions[Avatar::NUMBER_OF_AVATAR_CLASSES],
    Avatar::AvatarClassServerDescription
      server_descriptions[Avatar::NUMBER_OF_AVATAR_CLASSES],
    const dc::dcGenericResource::Array& spells) {
  for (int avatar_class = 0;
           avatar_class < NUMBER_OF_AVATAR_CLASSES;
         ++avatar_class) {
    classes_[avatar_class].compile(&client_descriptions[avatar_class],
                                   &server_descriptions[avatar_class]);

    // Fill the server descriptions using the new spell system
    for (int level = 0; level < HIGHEST_AVATAR_LEVEL; ++level) {
      server_descriptions[avatar_class].spell_availability_mask[level].zero();
    }

    dc::dcGenericResource::Array::const_iterator i;
    for (i = spells.begin(); i != spells.end(); ++i) {
      Evidyon::Tools::SpellElement* spell
        = (Evidyon::Tools::SpellElement*)(*i);
      spell->addToClassSpellAvailabilityMaskAtAcquiredLevel(
        Evidyon::Avatar::AvatarClass(avatar_class),
        server_descriptions[avatar_class].spell_availability_mask,
        HIGHEST_AVATAR_LEVEL);
    }

    // collect spells up the levels--add each mask's bits to the one above it
    for (int level = 1; level < HIGHEST_AVATAR_LEVEL; ++level) {
      server_descriptions[avatar_class].spell_availability_mask[level].unionWith(
        &server_descriptions[avatar_class].spell_availability_mask[level-1]);
    }

  }
}




}
}
}

