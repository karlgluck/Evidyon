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