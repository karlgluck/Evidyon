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
#ifndef __SHARED_SPELL_SPELLSTAGESERVERDESCRIPTION_H__
#define __SHARED_SPELL_SPELLSTAGESERVERDESCRIPTION_H__
#pragma once

#include "shared/magic/magicindex.h"
#include "shared/time/milliseconds.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarclass.h"

namespace Evidyon {
namespace Spell {


//----[  SpellStageServerDescription  ]----------------------------------------
struct SpellStageServerDescription {

  // When does each class get this level of this spell?
  Avatar::AvatarLevel level_obtained[Avatar::NUMBER_OF_AVATAR_CLASSES];

  // If the avatar's level is greater than this value, the base DPS is
  // calculated from this value.  Until then it's based on avatar's level.
  Avatar::AvatarLevel improvement_level_limit;

  // What kind of magic to use when this spell is cast
  Magic::MagicIndex magic_index;

  // How long after casting before another effect/action can be executed.
  Time::Milliseconds action_delay;
};


}
}

#endif