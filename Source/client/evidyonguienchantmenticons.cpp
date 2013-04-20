//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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