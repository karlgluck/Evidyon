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
#include "avatarclassspells.h"
#include <stdio.h> // for sprintf


namespace Evidyon {
namespace Spell {
namespace Tools {


//----[  fillClassSpellAvailabilityMask  ]-------------------------------------
void AvatarClassSpells::fillClassSpellAvailabilityMask(
  Evidyon::Avatar::SpellAvailabilityMask mask[Evidyon::Avatar::HIGHEST_AVATAR_LEVEL]) const {
  // Reset the mask
  for (int i = 0; i < Evidyon::Avatar::HIGHEST_AVATAR_LEVEL; ++i)
      mask[i].zero();

  // Fill in effects for their given level
  const dc::dcGenericResource::Array& members = getMembers();
  for (dcGenericResource::Array::const_iterator i = members.begin(); i != members.end(); ++i)
  {
    const Element* element = (Element*)(*i);
    int levelIndex = element->getLevelValue() - 1;
    levelIndex = levelIndex > 0 ? levelIndex : 0;
    if (APP_WARNING(levelIndex >= Evidyon::Avatar::HIGHEST_AVATAR_LEVEL)("Invalid level for magic (%lu being capped at %lu)", levelIndex+1, Evidyon::Avatar::HIGHEST_AVATAR_LEVEL ))
        levelIndex = Evidyon::Avatar::HIGHEST_AVATAR_LEVEL - 1;
    const dc::dcList<Evidyon::Spell::Tools::Spell>::Element* spell = element->getSpell();
    if (APP_WARNING(!spell)("Invalid spell at level %lu being ignored", levelIndex+1)) continue;
    unsigned int spellIndex = spell->getIndex();

    // Make this spell available
    mask[levelIndex].setBit(spellIndex);
  }

  // Go through each level and make sure that level has the previous level's abilities
  for (int i = 1; i < Evidyon::Avatar::HIGHEST_AVATAR_LEVEL; ++i)
      mask[i].unionWith(&mask[i-1]);
}




//----[  allowDuplicates  ]----------------------------------------------------
bool AvatarClassSpells::allowDuplicates() const
{
    return true;
}





//----[  keepSorted  ]---------------------------------------------------------
bool AvatarClassSpells::keepSorted() const
{
    return true;
}


//----[  AvatarClassSpells::Element  ]-----------------------------------------
AvatarClassSpells::Element::Element()
{
    member("Level", &level_);
    member("Magic", &spell_);
}



//----[  compareTo  ]----------------------------------------------------------
int AvatarClassSpells::Element::compareTo(const dc::dcGenericResource* other) const
{
    if (other)
    {
        if (getTypeID() == other->getTypeID())
            return level_.compareTo(&((Element*)other)->level_);
        else if (level_.getTypeID() == other->getTypeID())
            return level_.compareTo(other);
    }

    // Use the generic implementation
    return dc::dcGenericResource::compareTo(other);
}


//----[  getLevelValue  ]------------------------------------------------------
int AvatarClassSpells::Element::getLevelValue() const
{
    return level_.getValue();
}





//----[  getSpell  ]-----------------------------------------------------------
const dc::dcList<Evidyon::Spell::Tools::Spell>::Element* AvatarClassSpells::Element::getSpell() const
{
    return spell_.getReferencedResource();
}



//----[  set  ]----------------------------------------------------------------
void AvatarClassSpells::Element::set(int level,
                                     Evidyon::Tools::SpellElement* spell) {
  level_.setValue(level);
  spell_.setReferencedResource(spell);
  char name[512];
  sprintf_s(name, "%i - %s", level, spell ? spell->getName().c_str() : "??");
  setName(name);
}

}
}
}