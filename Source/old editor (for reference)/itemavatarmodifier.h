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
#ifndef __ITEMAVATARMODIFIER_H__
#define __ITEMAVATARMODIFIER_H__
#pragma once

#include <dc/integer>
#include <dc/float>
#include <dc/list>
#include "spell.h"
#include "itemavatarmodifierfield.h"
#include "itemavataractortypemodifier.h"


class ItemAvatarModifier : public dc::dcResource<ItemAvatarModifier> {
public:
  ItemAvatarModifier();

  // Builds text for what this item does based on its avatar modifier fields.
  // The first text is appended to the item's description.  The second text
  // is appended client-side when the corresponding bit is set in the item's
  // special properties.
  // This can fail if more than 8 fields are set to "prop" fields.
  bool compileForClient(bool is_weapon,
                        std::string* description_text,
                        std::string prop_field_descriptions[8]) const;

  // Builds the server's description of this item's modifier
  bool compileForServer(bool is_weapon,
                        Evidyon::CompiledItemAvatarModifier* modifier) const;


  void getCSVLine(std::string* output) const;

public:
  static std::string staticTypeName();


private:
  ItemAvatarModifierField<dc::dcInteger> min_attack_damage_;
  ItemAvatarModifierField<dc::dcInteger> max_attack_damage_;
  ItemAvatarModifierField<dc::dcInteger> physical_defense_;
  ItemAvatarModifierField<dc::dcInteger> magical_defense_;
  ItemAvatarModifierField<dc::dcInteger> hp_regen_rate_;
  ItemAvatarModifierField<dc::dcInteger> mp_regen_rate_;
  ItemAvatarModifierField<dc::dcFloat> hp_multiplier_;
  ItemAvatarModifierField<dc::dcFloat> mp_multiplier_;
  ItemAvatarModifierField<dc::dcInteger> hp_added_;
  ItemAvatarModifierField<dc::dcInteger> mp_added_;
  ItemAvatarModifierField<dc::dcInteger> level_added_;
  ItemAvatarModifierField<dc::dcFloat> movement_speed_multiplier_;
  ItemAvatarModifierField<dc::dcList<Spell>::Reference> spell_to_make_available_[2];
  ItemAvatarModifierField<dc::dcFloat> attack_speed_multiplier_;
  ItemAvatarModifierField<ItemAvatarActorTypeModifier> actor_type_;
  ItemAvatarModifierField<dc::dcFloat> steal_hp_percent_;
  ItemAvatarModifierField<dc::dcFloat> steal_mp_percent_;
};


#endif