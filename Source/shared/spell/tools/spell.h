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
#ifndef __SHARED_SPELL_TOOLS_SPELL_H__
#define __SHARED_SPELL_TOOLS_SPELL_H__
#pragma once

#include <dc/string>
#include <dc/float>
#include <dc/boolean>
#include <dc/integer>
#include <dc/enum>
#include "shared/actor/actoraction.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarclass.h"
#include <dc/list>
#include "spellstage.h"


namespace Evidyon {
namespace Avatar {
struct SpellAvailabilityMask;
}
namespace Magic {
class MagicCompiler;
}
namespace Spell {
struct SpellClientDescription;
struct SpellServerDescription;
}
}



namespace Evidyon {
namespace Spell {
namespace Tools {



//----[  Spell  ]--------------------------------------------------------------
class Spell : public dc::dcResource<Spell> {
public:
  static std::string staticTypeName() { return "Spell"; }
  Spell();
  void compile(Magic::Tools::MagicCompiler* magic_compiler,
               SpellClientDescription* client_description,
               SpellServerDescription* server_description);
public:

  // this method is used when creating spells of a certain type
  void implement(const std::string& name,
                 unsigned long type);

  // used to configure this class based on values in the file imported from the
  // master design document.
  void set(std::map<std::string,std::string>* values);

  // Finds the level at which the given class gets this spell.  If the class
  // doesn't get this spell, the return value is 'false'.
  bool getLevelAcquired(Avatar::AvatarClass avatar_class,
                        Avatar::AvatarLevel* level);

  // After this method is called for all spells by a class, the class should go
  // through the masks it generated and OR-up the masks so that spells acquired
  // at earlier levels are marked as available at later levels.
  // The masks are assumed to be zero-based, so level 1 is at index 0.
  void addToClassSpellAvailabilityMaskAtAcquiredLevel(
    Avatar::AvatarClass avatar_class,
    Avatar::SpellAvailabilityMask* level_masks,
    size_t number_of_level_masks);

private:
  dc::dcString name_;
  dc::dcString description_;
  dc::dcInteger mp_cost_;
  dc::dcBoolean casting_requires_target_,
                casting_requires_slot_,
                can_target_caster_,
                can_target_dead_;
  dc::dcEnum<Actor::ActorAction> casting_action_;

  Evidyon::Tools::SpellStageList stages_;
};


}
}
}

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Spell::Tools::Spell> SpellList;
typedef dc::dcList<Evidyon::Spell::Tools::Spell>::Element SpellElement;
typedef dc::dcList<Evidyon::Spell::Tools::Spell>::Reference SpellReference;
}
}

#endif
