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
#ifndef __SHARED_SPELL_TOOLS_AVATARCLASSSPELLS_H__
#define __SHARED_SPELL_TOOLS_AVATARCLASSSPELLS_H__
#pragma once

#include <dc/integer>
#include <dc/list>
#include "shared/spell/tools/spell.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/spellavailabilitymask.h"

namespace Evidyon {
namespace Spell {
namespace Tools {


//----[  AvatarClassSpells  ]--------------------------------------------------
class AvatarClassSpells : public dc::dcCollection<AvatarClassSpells> {
public:
  static std::string staticTypeName() { return "AvatarClassSpells"; }

public:
  //----[  Element  ]----------------------------------------------------------
  class Element : public dc::dcResource<Element> {
  public:
    static std::string staticTypeName() { return "Element of AvatarClassSpell"; }
  public:
    Element();
    int compareTo(const dc::dcGenericResource* other) const;
    int getLevelValue() const;
    const dc::dcList<Evidyon::Spell::Tools::Spell>::Element* getSpell() const;
    void set(int level, Evidyon::Tools::SpellElement* effect);

  protected:
    dc::dcInteger level_;  // The level of this spell
    Evidyon::Tools::SpellReference spell_;  // The spell to make available
  };

public:
  virtual bool allowDuplicates() const;
  virtual bool keepSorted() const;

  // Fills out the mask with available spells
  void fillClassSpellAvailabilityMask(
    Evidyon::Avatar::SpellAvailabilityMask mask[Evidyon::Avatar::HIGHEST_AVATAR_LEVEL]) const;
};


}
}
}

#endif