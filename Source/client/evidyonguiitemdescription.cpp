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
#include "evidyonguiitemdescription.h"
#include "common/gui/guicanvas.h"
#include "common/gui/guifont.h"
#include "common/gui/guilayer.h"
#include <string>
#include "shared/item/itemclientdescription.h"

namespace Evidyon {


/*
common-white
uncommon-blue
rare-yellow/gold
epic-red
*/
  D3DCOLOR GetItemRarityColor(const Item::ItemClientDescription* item) {
  using namespace Evidyon::Item;
  D3DCOLOR color;
  switch (item->rarity) {
    default:
    case ITEMRARITY_COMMON: color = 0xFFFFFFFF; break;
    case ITEMRARITY_UNCOMMON: color = 0xFFFF7F00; break;
    case ITEMRARITY_RARE: color = 0xFFFFFF00; break;
    case ITEMRARITY_EPIC: color = 0xFFFF0000; break;
  }
  return color;
}


void EvidyonGUIItemDescription::setFont(GUIFont* font) {
  description_.setFont(font);
}


void EvidyonGUIItemDescription::setItem(const ItemInStorage* storage_item,
                                        const Item::ItemClientDescription* item_description,
                                        GUISize* panel_size) {
  std::string text;
  text.append(item_description->description);
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 40;
    size.width = max(size.width, 120);
    const int min_height = font->getCaretSize().height*3;
    size.height = max(min_height, size.height);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
  description_.setTextColor(GetItemRarityColor(item_description));
}

void EvidyonGUIItemDescription::setItem(const Item::ItemClientDescription* item_description,
                                        GUISize* panel_size) {
  std::string text;
  text.append(item_description->description);
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 40;
    size.width = max(size.width, 120);
    const int min_height = font->getCaretSize().height*3;
    size.height = max(min_height, size.height);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
  description_.setTextColor(GetItemRarityColor(item_description));
}

void EvidyonGUIItemDescription::setItem(const char* prefix_text,
                                        const Item::ItemClientDescription* item_description,
                                        GUISize* panel_size) {
  std::string text;
  if (prefix_text) {
    text = prefix_text;
  }
  text.append(item_description->description);
  {
    GUIFont* font = description_.getFont();
    GUISize size;
    font->getTextExtent(text.c_str(), text.length(), &size);
    size.width += 40;
    size.width = max(size.width, 120);
    const int min_height = font->getCaretSize().height*3;
    size.height = max(min_height, size.height);
    size.width += 16;
    size.height += 16;
    *panel_size = size;
  }
  description_.setText(text.c_str());
  description_.setAlignment(GUIALIGN_LEFT_TOP);
  description_.setTextColor(GetItemRarityColor(item_description));
}


void EvidyonGUIItemDescription::registerElements(GUILayer* layer) {
  GUICanvas* canvas = getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYCenter(0);
  canvas->relativeWidth(-16);
  canvas->relativeHeight(-16);
  canvas->relativeDepth(-0.01f);
  description_.setSourceCanvas(canvas);
  layer->registerElement(&description_);
}

void EvidyonGUIItemDescription::unregisterElements(GUILayer* layer) {
  layer->unregisterElement(&description_);
}





}

