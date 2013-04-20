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
#include "stdafx.h"
#include "wavsound.h"

#include "avatarmodifier.h"
#include "playercharacterclasslevelspells.h"
#include "playercharacterclass.h"







//------------------------------------------------------------------------------------------------
// Name: PlayerCharacterClass
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
PlayerCharacterClass::PlayerCharacterClass() {
  member("Description", &myDescription);

  member("HP Base Factor (Str)", &myHPStrBase);
  member("HP Base Factor (Con)", &myHPConBase);
  member("HP Modifier (Str)", &myHPStrModifier);
  member("HP Modifier (Con)", &myHPConModifier);
  member("HP Bonus Increment (Str)", &myHPStrBonusIncrement);
  member("HP Bonus Increment (Con)", &myHPConBonusIncrement);

  member("MP Base Factor (Int)", &myMPIntBase);
  member("MP Base Factor (Wis)", &myMPWisBase);
  member("MP Modifier (Int)", &myMPIntModifier);
  member("MP Modifier (Wis)", &myMPWisModifier);
  member("MP Bonus Increment (Int)", &myMPIntBonusIncrement);
  member("MP Bonus Increment (Wis)", &myMPWisBonusIncrement);

  member("Spells", &spells_);
}



//------------------------------------------------------------------------------------------------
// Name: toString
// Desc: Gets the string representation of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClass::toString() {
  return getName();
}




//------------------------------------------------------------------------------------------------
// Name: fillServerDescription
// Desc: Fills out this structure's description for the server
//------------------------------------------------------------------------------------------------
void PlayerCharacterClass::fillServerDescription(AvatarClassDescription* description) const {
  description->hp_str_base = myHPStrBase.getValue();
  description->hp_str_bonus_increment = myHPStrBonusIncrement.getValue();
  description->hp_str_modifier = myHPStrModifier.getValue();

  description->hp_con_base = myHPConBase.getValue();
  description->hp_con_bonus_increment = myHPConBonusIncrement.getValue();
  description->hp_con_modifier = myHPConModifier.getValue();

  description->mp_int_base = myMPIntBase.getValue();
  description->mp_int_bonus_increment = myMPIntBonusIncrement.getValue();
  description->mp_int_modifier = myMPIntModifier.getValue();

  description->mp_wis_base = myMPWisBase.getValue();
  description->mp_wis_bonus_increment = myMPWisBonusIncrement.getValue();
  description->mp_wis_modifier = myMPWisModifier.getValue();

  spells_.fillClassSpellAvailabilityMask(description->spellAvailabilityMask);
}


//------------------------------------------------------------------------------------------------
// Name: getDescription()
// Desc: Obtains the client description for this classs
//------------------------------------------------------------------------------------------------
const std::string& PlayerCharacterClass::getDescription() const {
  return myDescription.getValue();
}


//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: The name of this resource
//------------------------------------------------------------------------------------------------
std::string PlayerCharacterClass::staticTypeName() {
  return "Player Character Class";
}


