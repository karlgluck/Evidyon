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
#ifndef __ITEM_H__
#define __ITEM_H__
#pragma once

namespace Evidyon {
struct ServerItemDescription;
}

struct ClientItemDescription;
#include <dc/list>
#include "scenery.h"
#include <dc/enum>
#include <dc/string>
#include "spell.h"



#include "itemavatarmodifier.h"

/**
 * 
 */
class Item : public dc::dcResource<Item>
{
public:

  /**
   * Initializes this class
   */
  Item();

  /**
   * Enters data into the server effect structure
   */
  bool fillServerDescription(ServerItemDescription* description) const;

  /**
   * Enters data into the client effect structure
   */
  bool compileForClient(ClientItemDescription* description) const;

  void createCSVLine(char* buffer, size_t buffer_size) const;

  /**
   * Sets the scenery visualization of this item
   */
  void setSceneryRepresentation(dcList<Scenery>::Element* scenery);

  dc::dcList<Scenery>::Reference* getSceneryReference() { return &mySceneryRepresentation; }

  /**
   * Used to sort this list.  If the static "compiling" flag is set, the items are
   * sorted by type ID.  Otherwise, they are sorted by their equipment slot.
   */
  inline int getTreasureLevelValue() const { return treasure_level_.getValue(); }

  inline dc::dcEnum<EquipmentSlot>& getSlot() { return myEquipmentSlot; }

  inline ItemAvatarModifier& getItemAvatarModifier() { return avatar_modifier_; }

public:

  /**
   * Returns the type name of this class
   */
  static std::string staticTypeName();


protected:

  // this is the name that the item is given in the client's game file.
  dc::dcString displayed_name_;

  dc::dcEnum<EquipmentSlot> myEquipmentSlot;  /// Which slot in the "equipped items" region to place this item in
  dc::dcString myDescription; /// The textual description of this item
  dc::dcList<Scenery>::Reference mySceneryRepresentation; /// What this item looks like when it is on the ground (or item preview in inventory)

  dc::dcByte max_stack_size_;
  //dc::dcFloat weight_;

  dc::dcInteger treasure_level_;  // how rare this item is
  dc::dcInteger geonite_value_;    // amount of geonite gained when sacrificed

  dc::dcTable<Magic>::Reference consumable_magic_;
  dc::dcBoolean consumable_needs_target_;
  ItemAvatarModifier avatar_modifier_; // what this item does
};





#endif