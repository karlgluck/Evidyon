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
#ifndef __SHARED_SPELL_TOOLS_SPELLSTAGE_H__
#define __SHARED_SPELL_TOOLS_SPELLSTAGE_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include <dc/integer>
#include "shared/avatar/avatarclass.h"
#include "shared/avatar/avatarlevel.h"

namespace Evidyon {
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
namespace Spell {
struct SpellStageServerDescription;
}
}

namespace Evidyon {
namespace Spell {
namespace Tools {


//----[  SpellStage  ]---------------------------------------------------------
class SpellStage : public dc::dcResource<SpellStage> {
public:
  static std::string staticTypeName() { return "SpellStage"; }

public:
  SpellStage();
  void compile(Magic::Tools::MagicCompiler* magic_compiler,
               Evidyon::Spell::SpellStageServerDescription* description);

  // used to configure this class based on values in the file imported from the
  // master design document.
  void set(std::map<std::string,std::string>* values);

  // Finds the level at which the given class gets this stage of this
  // spell.  If the class doesn't get this stage, the return value is 'false'
  // and *level = 0
  bool getLevelAcquired(Avatar::AvatarClass avatar_class,
                        Avatar::AvatarLevel* level);

private:
  Evidyon::Tools::MagicInterface magic_;
  dc::dcInteger level_obtained_[Avatar::NUMBER_OF_AVATAR_CLASSES];
  dc::dcInteger improvement_level_limit_;
  dc::dcInteger action_delay_ms_;
};

}
}
}


#include <dc/list>

namespace Evidyon {
namespace Tools {
typedef dc::dcList<Evidyon::Spell::Tools::SpellStage> SpellStageList;
typedef dc::dcList<Evidyon::Spell::Tools::SpellStage>::Element SpellStageElement;
typedef dc::dcList<Evidyon::Spell::Tools::SpellStage>::Reference SpellStageReference;
}
}



#endif