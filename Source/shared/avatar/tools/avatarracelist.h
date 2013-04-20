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
#ifndef __SHARED_AVATAR_TOOLS_AVATARRACELIST_H__
#define __SHARED_AVATAR_TOOLS_AVATARRACELIST_H__
#pragma once

#include "avatarrace.h"
#include "shared/avatar/avatarrace.h"


namespace Evidyon {
namespace Avatar {
namespace Tools {


  
//----[  AvatarRaceList  ]-----------------------------------------------------
class AvatarRaceList : public dc::dcResource<AvatarRaceList> {
public:
  static std::string staticTypeName() { return "AvatarRaceList"; }
public:
  AvatarRaceList();
  bool compile(Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
               Avatar::AvatarRaceClientDescription
                 client_descriptions[Avatar::NUMBER_OF_AVATAR_RACES],
               Avatar::AvatarRaceServerDescription
                 server_descriptions[Avatar::NUMBER_OF_AVATAR_RACES]);
  void getForm(Avatar::AvatarRace race,
               Avatar::AvatarGender gender,
               Evidyon::Actor::Tools::ActorFormReference* form);

private:
  AvatarRace races_[Avatar::NUMBER_OF_AVATAR_RACES];
};




}
}
}

#endif