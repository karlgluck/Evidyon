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
#include "itemavataractortypemodifier.h"

// needed so we can get default actor types for invalid references
// NOTE: this is TODO because i haven't implemented changing actor
// based on class type yet--only race, and i don't want to implement
// something that requires a lot of code to be written that i'm just
// going to have to rewrite when that change is made.
#include "stdafx.h"
#include "allgameresources.h"
#include "editresourcedialog.h"
#include "projectvolucriseditor.h"


ItemAvatarActorTypeModifier::ItemAvatarActorTypeModifier() {
  using namespace Evidyon;
  for (int gender_index = 0; gender_index < AVATARGENDER_COUNT; ++gender_index) {
    for (int race_index = 0; race_index < AVATARRACE_COUNT; ++race_index) {
      for (int class_index = 0; class_index < AVATARCLASS_COUNT; ++class_index) {
        std::string gender = AvatarGenderString((AvatarGender)gender_index);
        std::string race = AvatarRaceString((AvatarRace)race_index);
        std::string class_name = AvatarClassString((AvatarClass)class_index);
        member(gender + " " + race + " " + class_name,
               &actor_types_[gender_index][race_index][class_index]);
      }
    }
  }
}


void ItemAvatarActorTypeModifier::compileForServer(int actor_indices[Avatar::NUMBER_OF_AVATAR_GENDERS]
                                                                    [Avatar::NUMBER_OF_AVATAR_RACES]
                                                                    [Avatar::NUMBER_OF_AVATAR_CLASSES]) const {
  using namespace Evidyon;
  ProjectVolucris* pv = globalEditor()->getRootResource();
  for (int gender_index = 0; gender_index < AVATARGENDER_COUNT; ++gender_index) {
    for (int race_index = 0; race_index < AVATARRACE_COUNT; ++race_index) {
      for (int class_index = 0; class_index < AVATARCLASS_COUNT; ++class_index) {
        const dc::dcList<ActorType>::Element* actor_element =
            actor_types_[gender_index][race_index][class_index].getReferencedResource();

        if (actor_element == NULL) {
          // TODO!
          actor_indices[gender_index][race_index][class_index] = 0;
        } else {
          actor_indices[gender_index][race_index][class_index] =
              actor_element->getIndex();
        }
      }
    }
  }
}




std::string ItemAvatarActorTypeModifier::staticTypeName() {
  return "ItemAvatarActorTypeModifier";
}