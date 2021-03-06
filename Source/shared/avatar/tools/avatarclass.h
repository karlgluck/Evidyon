//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#ifndef __SHARED_AVATAR_TOOLS_AVATARCLASS_H__
#define __SHARED_AVATAR_TOOLS_AVATARCLASS_H__
#pragma once

#include <dc/float>
#include <dc/string>
#include "shared/spell/tools/avatarclassspells.h"

namespace Evidyon {
namespace Avatar {
struct AvatarClassClientDescription;
struct AvatarClassServerDescription;
}
}



namespace Evidyon {
namespace Avatar {
namespace Tools {


  
//----[  AvatarClass  ]--------------------------------------------------------
class AvatarClass : public dc::dcResource<AvatarClass> {
public:
  static std::string staticTypeName() { return "AvatarClass"; }
public:
  AvatarClass();
  void compile(Avatar::AvatarClassClientDescription* client_description,
               Avatar::AvatarClassServerDescription* server_description);
public:
  dc::dcString description_;
  Spell::Tools::AvatarClassSpells spells_;
};

}
}
}

#endif