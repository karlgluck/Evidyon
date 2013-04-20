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
#ifndef __SHARED_AVATAR_TOOLS_AVATARRACE_H__
#define __SHARED_AVATAR_TOOLS_AVATARRACE_H__
#pragma once

// for AVATAR_RACE_HAIR_STYLES_PER_GENDER
#include "shared/avatar/avatarrace.h"

#include "shared/avatar/avatargender.h"
#include "shared/avatar/avatarattribute.h"
#include <dc/byte>
#include <dc/string>
#include "shared/actor/tools/actorformreference.h"
#include "shared/scenery/tools/scenery.h"


namespace Evidyon {
namespace Actor {
namespace Tools {
class ActorTemplateCompiler;
}
}
namespace Avatar {
struct AvatarRaceClientDescription;
struct AvatarRaceServerDescription;
}
}

namespace Evidyon {
namespace Avatar {
namespace Tools {



//----[  AvatarRace  ]---------------------------------------------------------
class AvatarRace : public dc::dcResource<AvatarRace> {
public:
  static std::string staticTypeName() { return "AvatarRace"; }
public:
  AvatarRace();
  bool compile(Actor::Tools::ActorTemplateCompiler* actor_template_compiler,
               Avatar::AvatarRaceClientDescription* client_description,
               Avatar::AvatarRaceServerDescription* server_description);
  void getForm(Avatar::AvatarGender gender,
               Evidyon::Actor::Tools::ActorFormReference* form);
private:
  dc::dcString description_;
  dc::dcByte attribute_level_1_maxes_[NUMBER_OF_AVATAR_ATTRIBUTES];
  Evidyon::Actor::Tools::ActorFormReference
    gender_actors_[NUMBER_OF_AVATAR_GENDERS];
  Evidyon::Tools::SceneryReference
    hair_styles_[NUMBER_OF_AVATAR_GENDERS]
                [AVATAR_RACE_HAIR_STYLES_PER_GENDER];
};

}
}
}

#endif