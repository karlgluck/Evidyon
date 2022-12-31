//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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