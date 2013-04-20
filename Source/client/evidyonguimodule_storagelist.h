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
#ifndef __EVIDYONGUIMODULE_STORAGELIST_H__
#define __EVIDYONGUIMODULE_STORAGELIST_H__
#pragma once


#include "evidyonguiscrollablebuttonlist.h"
#include "common/gui/guimodule.h"
#include "evidyonguiitemdescription.h"
#include "shared/evidyon_storage.h"
#include "common/gui/guitextlabelcomponent.h"

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

class EvidyonGUITooltipBox;
class EvidyonGUIKeyBoundActionsSentinel;

class EvidyonGUIModule_StorageList : public GUIModule {
public:
  EvidyonGUIModule_StorageList();

  void create(GUICanvas* source_canvas,
              GUIFont* button_font,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ItemInStorage* storage,
              EvidyonGUITooltipBox* tooltip);
  void destroy();

  void resetStorageContent();

  GUICanvas* getCanvas();

  // Updates the list with the current state of the storage
  void sync();

  bool getMouseOverItem(const ItemInStorage** storage_item,
                        const Item::ItemClientDescription** item_description);
  bool getClickedItem(const ItemInStorage** storage_item,
                      const Item::ItemClientDescription** item_description);

  void updateTooltip();

  void scrollToTop();

private:
  GUIFont* button_font_;
  const ItemInStorage* storage_;
  EvidyonGUIScrollableButtonList item_list_;
  const Item::ItemClientDescription* item_descriptions_;
  size_t number_of_item_descriptions_;

  GUITextLabelComponent label_;

  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIItemDescription tooltip_description_;
};

}



#endif