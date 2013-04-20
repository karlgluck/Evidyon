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
#ifndef __PLAYERCHARACTERCLASS_H__
#define __PLAYERCHARACTERCLASS_H__
#pragma once


#include <dc/float>
#include "playercharacterclasslevelspells.h"
#include "../shared/avatar/evidyon_avatar.h"

namespace Evidyon {
struct AvatarClassDescription;
}

/**
 * Specifies a character class (fighter, paladin, wizard, etc...) and its related attributes
 */
class PlayerCharacterClass : public dc::dcResource<PlayerCharacterClass> {
public:

  // Initializes this class
  PlayerCharacterClass();

  // Gets the string representation of this resource
  std::string toString();

  // Fills out this structure's description for the server
  void fillServerDescription(Evidyon::AvatarClassDescription* description) const;

  const std::string& getDescription() const;


public:

  /**
   * The name of this resource
   */
  static std::string staticTypeName();


protected:

  // Description of the race
  dc::dcString myDescription;

  dc::dcFloat myHPStrBase, myHPConBase,
              myHPStrModifier, myHPConModifier,
              myHPStrBonusIncrement, myHPConBonusIncrement;
  dc::dcFloat myMPIntBase, myMPWisBase,
              myMPIntModifier, myMPWisModifier,
              myMPIntBonusIncrement, myMPWisBonusIncrement;

  /// The effects that players of this class are able to use at a particular level
  PlayerCharacterClassLevelSpells spells_;
};


#endif