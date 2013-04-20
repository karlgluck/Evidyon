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
#ifndef __EVIDYONGUIMODULE_BAZAARCATEGORYITEMS_H__
#define __EVIDYONGUIMODULE_BAZAARCATEGORYITEMS_H__
#pragma once

#include "evidyonguiactionbuttonlist.h"
#include "evidyonguiitemdescription.h"
#include "common/gui/guitextlabelcomponent.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/client-server/bazaar.h"

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

class EvidyonGUIModule_Bazaar;
struct ItemInBazaar;
enum BazaarCategory;

class EvidyonGUIModule_BazaarCategoryItems : public EvidyonGUIActionButtonList {
public:

  // Sets up the list, including initializing its canvas.
  void create(EvidyonGUIModule_Bazaar* bazaar,
              GUICanvas* source_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  // Called when the category first switches.  Clears the current set of displayed
  // items in preparation for a list from the server.
  void changeCategory(BazaarCategory category);

  // Sets the items in this category if the category matches the one currently
  // being displayed.  This is a precaution to prevent the wrong page from being
  // shown if messages get out of order or the user flips between screens quickly.
  void sync(BazaarCategory category,
            ItemInBazaar* items_in_category,
            size_t number_of_items);

  // Gets the set of items that are currently visible
  BazaarCategory getCurrentCategory();


private:
  virtual void onButtonClicked(int button_id, ActionTargeter* action_targeter);
  virtual bool updateTooltipContent(int button_id, EvidyonGUITooltipBox* tooltip);
  virtual void addTooltipContent(EvidyonGUITooltipBox* tooltip);


private:
  EvidyonGUIModule_Bazaar* bazaar_;
  BazaarCategory current_category_;
  const Item::ItemClientDescription* item_descriptions_;
  size_t number_of_item_descriptions_;
  EvidyonGUIItemDescription tooltip_description_;
  ItemInBazaar items_[MAX_ITEMS_PER_BAZAAR_CATEGORY];
  size_t number_of_items_;
  GUITextLabelComponent label_;

};




}



#endif