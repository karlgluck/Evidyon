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
#ifndef __ITEMAVATARACTORTYPEMODIFIER_H__
#define __ITEMAVATARACTORTYPEMODIFIER_H__
#pragma once

#include <dc/list>
#include "actortype.h"
#include "../shared/avatar/evidyon_avatar.h"
#include "../avatar/avatarrace.h"
#include "../avatar/avatarclass.h"

// This class is mainly used so that armor can change the appearance of a character
class ItemAvatarActorTypeModifier : public dc::dcResource<ItemAvatarActorTypeModifier> {
public:
  ItemAvatarActorTypeModifier();
  void compileForServer(int actor_indices[Avatar::NUMBER_OF_AVATAR_GENDERS]
                                         [Avatar::NUMBER_OF_AVATAR_RACES]
                                         [Avatar::NUMBER_OF_AVATAR_CLASSES]) const;

public:
  static std::string staticTypeName();

private:
  dc::dcList<ActorType>::Reference actor_types_[Avatar::NUMBER_OF_AVATAR_GENDERS]
                                               [Avatar::NUMBER_OF_AVATAR_RACES]
                                               [Avatar::NUMBER_OF_AVATAR_CLASSES];
};


#endif