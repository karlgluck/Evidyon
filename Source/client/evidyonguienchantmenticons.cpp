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
#include "evidyonguienchantmenticons.h"
#include "evidyonguidefs.h"
#include "common/gui/guilayer.h"
#include <assert.h>

namespace Evidyon {

static const int ICON_SIZE = 32;

  
//----[  EvidyonGUIEnchantmentIcons  ]-----------------------------------------
EvidyonGUIEnchantmentIcons::EvidyonGUIEnchantmentIcons() {
}




//----[  init  ]---------------------------------------------------------------
void EvidyonGUIEnchantmentIcons::init(GUICanvas* primary_canvas,
                                      GUISize hp_mp_xp_size) {
  GUICanvas* canvas = getCanvas();
  canvas->setSourceCanvas(primary_canvas);
  canvas->relativeWidth(0);
  canvas->relativeHeight(0);
  canvas->alignXLeft(0);
  canvas->alignYTop(0);
  canvas->absoluteDepth(0.9f);

  canvas_matrix_.setSourceCanvas(canvas);
  canvas_matrix_.relativeDepth(0.0f);
  canvas_matrix_.alignXLeft(8 + hp_mp_xp_size.width);
  canvas_matrix_.alignYBottom(-4);
  canvas_matrix_.absoluteWidth(Magic::NUMBER_OF_BUFFS * (ICON_SIZE+4));
  canvas_matrix_.absoluteHeight(ICON_SIZE);

  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    icons_[i].setSourceCanvas(canvas_matrix_.getCellCanvas(0, i));
    icons_[i].setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
  }

  update(NULL);
}




//----[  update  ]-------------------------------------------------------------
void EvidyonGUIEnchantmentIcons::update(const Magic::BuffMask* mask) {
  int icon_index = 0;

  // Set the active icons
  if (mask != NULL) {
    for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
      Magic::Buff buff = (Magic::Buff)i;
      if (mask->get(buff)) {
        GUIRect area;
        getIconSrcRect(buff, &area);
        icons_[icon_index].setSourceRegion(&area, EvidyonGUIDefs::TEXTURE_SIZE);
        ++icon_index;
      }
    }
  }

  // Clear the unused icons
  GUIRect zero_area = { 0, 0, 0, 0 };
  while (icon_index < Magic::NUMBER_OF_BUFFS) {
    icons_[icon_index++].setSourceRegion(&zero_area, EvidyonGUIDefs::TEXTURE_SIZE);
  }
}




//----[  registerElements  ]---------------------------------------------------
void EvidyonGUIEnchantmentIcons::registerElements(GUILayer* layer) {
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    layer->registerElement(&icons_[i]);
  }
}




//----[  unregisterElements  ]-------------------------------------------------
void EvidyonGUIEnchantmentIcons::unregisterElements(GUILayer* layer) {
  for (int i = 0; i < Magic::NUMBER_OF_BUFFS; ++i) {
    layer->unregisterElement(&icons_[i]);
  }
}




//----[  getIconSrcRect  ]-----------------------------------------------------
void EvidyonGUIEnchantmentIcons::getIconSrcRect(Magic::Buff buff, GUIRect* rect) {
  GUIPoint pt;
  switch (buff) {
  using namespace Evidyon::Magic;
  default:
    assert(false);
  case BUFF_MAX_HP:
  case BUFF_MAX_MP:
  case BUFF_DEFENSE:
    pt.x = 2*32;
    pt.y = 3*32 + 0*ICON_SIZE;
    break;
  case BUFF_MELEE_DAMAGE:
  case BUFF_MAGIC_DAMAGE:
    pt.x = 2*32;
    pt.y = 3*32 + 1*ICON_SIZE;
    break;
  case BUFF_HP_STEAL:
  case BUFF_MP_STEAL:
    pt.x = 2*32;
    pt.y = 3*32 + 2*ICON_SIZE;
    break;
  case BUFF_ACTION_SPEED_MODIFIER:
  case BUFF_MOVEMENT_SPEED:
    pt.x = 2*32;
    pt.y = 3*32 + 3*ICON_SIZE;
    break;
  case BUFF_HP_REGEN:
  case BUFF_POISON:
  case BUFF_MP_REGEN:
  case BUFF_LEECH:
    pt.x = 2*32;
    pt.y = 3*32 + 4*ICON_SIZE;
    break;
  }

  GUISize size = { ICON_SIZE, ICON_SIZE };
  rect->set(pt, size);
}




}