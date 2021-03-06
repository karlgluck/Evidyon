//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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
#ifndef __EVIDYONGUIMODULE_INVENTORYLIST_H__
#define __EVIDYONGUIMODULE_INVENTORYLIST_H__
#pragma once


#include "evidyonguiscrollablebuttonlist.h"
#include "common/gui/guimodule.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "evidyonguiitemdescription.h"
#include "common/gui/guitextlabelcomponent.h"

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_InventoryList : public GUIModule {
public:
  // Defines which of the inventory items this list displays.
  enum DisplayType {
    DISPLAY_CONSUMABLES,
    DISPLAY_EQUIPPED,
    DISPLAY_UNEQUIPPED,
  };

public:
  EvidyonGUIModule_InventoryList();

  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  GUICanvas* getCanvas();

  // Updates the list with the current state of the inventory
  void sync(DisplayType display_type,
            EvidyonGUIKeyBoundActionsSentinel* key_bindings);

  bool getMouseOverItem(const ClientAvatarInventoryItem** inventory_item,
                        const Item::ItemClientDescription** item_description);
  bool getClickedItem(const ClientAvatarInventoryItem** inventory_item,
                      const Item::ItemClientDescription** item_description);

  void updateTooltip();

  // Makes sure that the scroll bar is displaying at least a portion of
  // the list
  void validateScrollBar();

  void setText(const std::string& text);

  void scrollToTop();

  void setBackgroundColor(D3DCOLOR color);

private:
  GUIFont* button_font_;
  EvidyonGUIScrollableButtonList item_list_;
  const ClientAvatarInventoryItem* inventory_;
  const Item::ItemClientDescription* item_descriptions_;
  size_t number_of_item_descriptions_;

  GUITextLabelComponent label_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIItemDescription tooltip_description_;
};

}



#endif