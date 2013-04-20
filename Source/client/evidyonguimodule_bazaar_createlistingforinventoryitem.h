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
#ifndef __EVIDYONGUIMODULE_CREATELISTINGFORINVENTORYITEM_H__
#define __EVIDYONGUIMODULE_CREATELISTINGFORINVENTORYITEM_H__
#pragma once


#include "evidyonguimodule_actioninventoryitemlist.h"
#include "common/gui/guitextlabelcomponent.h"


namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}



namespace Evidyon {

class EvidyonGUIModule_Bazaar;


// Displays the client's inventory in the bazaar.  If an item is clicked,
// a prompt pops up that asks the user how much they want to charge for the
// item.  As they are typing in the field, the program calculates and
// displays how much commission the bazaar is going to charge.
class EvidyonGUIModule_Bazaar_CreateListingForInventoryItem : public EvidyonGUIModule_ActionInventoryItemList {
public:
  void create(EvidyonGUIModule_Bazaar* owner,
              GUICanvas* source_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  // Updates the displayed items
  void syncInventory();

private:

  // Only unequipped items are displayed
  virtual bool displayItem(const ClientAvatarInventoryItem* item,
                           const Item::ItemClientDescription* description,
                           std::string* name,
                           void* context_pointer);

  // The bazaar will prompt the user for a price
  virtual void onItemClicked(const ClientAvatarInventoryItem* item,
                             const Item::ItemClientDescription* item_description,
                             ActionTargeter* action_targeter);

private:
  EvidyonGUIModule_Bazaar* owner_;
  GUITextLabelComponent label_;
};





}


#endif