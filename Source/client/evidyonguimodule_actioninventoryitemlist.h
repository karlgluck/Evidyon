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
#ifndef __EVIDYONGUIMODULE_ACTIONINVENTORYITEMLIST_H__
#define __EVIDYONGUIMODULE_ACTIONINVENTORYITEMLIST_H__
#pragma once

#include "evidyonguiactionbuttonlist.h"
#include "evidyonguiitemdescription.h"
#include "common/gui/guitextlabelcomponent.h"
#include "shared/avatar/evidyon_avatarinventory.h"


namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}

namespace Evidyon {


// Makes it easy to make a list of items that causes an action to be performed
// when one is selected.
class EvidyonGUIModule_ActionInventoryItemList : public EvidyonGUIActionButtonList {
public:

  // Displays the inventory.  For each item, displayItem is checked to see if
  // the item should be put into the list.
  void sync(void* context_pointer);


protected:

  // Initializes the list.  Called by the subclass.  This method will
  // call EvidyonGUIActionButtonList::createActionButtonList.
  void createActionInventoryItemList(
      GUIFont* button_font,
      const Item::ItemClientDescription* item_descriptions,
      size_t number_of_item_descriptions,
      const ClientAvatarInventoryItem* inventory,
      EvidyonGUITooltipBox* tooltip);

  // Cleans up.  Called by the subclass.  This method invokes
  // EvidyonGUIActionButtonList::destroyActionButtonList.
  void destroyActionInventoryItemList();


private:
  // Implemented by this class
  virtual void onButtonClicked(int button_id, ActionTargeter* action_targeter);
  virtual bool updateTooltipContent(int button_id, EvidyonGUITooltipBox* tooltip);
  virtual void addTooltipContent(EvidyonGUITooltipBox* tooltip);

private:

  // Implemented by the derived class.  If the method returns 'true', the item
  // will be shown in the list.  The 'name' parameter is filled with the default
  // formatted name for this item when it is invoked.  If desired, the method can
  // change the item's name (for example, to display the key to which a consumable
  // item is bound).  The context pointer is whatever is passed into 'sync'.
  virtual bool displayItem(const ClientAvatarInventoryItem* item,
                           const Item::ItemClientDescription* description,
                           std::string* name,
                           void* context_pointer);

  // Implemented by the derived class to do whatever it needs to do when an
  // item is clicked.
  virtual void onItemClicked(const ClientAvatarInventoryItem* item,
                             const Item::ItemClientDescription* item_description,
                             ActionTargeter* action_targeter);

public:
  const ClientAvatarInventoryItem* inventory_;
  const Item::ItemClientDescription* item_descriptions_;
  size_t number_of_item_descriptions_;
  EvidyonGUIItemDescription tooltip_description_;
};




}



#endif