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
#ifndef __EVIDYONGUIITEMDESCRPITION_H__
#define __EVIDYONGUIITEMDESCRPITION_H__
#pragma once




#include "common/gui/guicomponent.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "common/gui/guitextlabelelement.h"

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

struct ItemInStorage;

class EvidyonGUIItemDescription : public GUIComponent {
public:
  void setFont(GUIFont* font);
  void setItem(const Item::ItemClientDescription* item_description,
               GUISize* panel_size);
  void setItem(const ItemInStorage* storage_item,
               const Item::ItemClientDescription* item_description,
               GUISize* panel_size);
  void setItem(const char* prefix_text,
               const Item::ItemClientDescription* item_description,
               GUISize* panel_size);

  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  GUITextLabelElement description_;
};

}



#endif