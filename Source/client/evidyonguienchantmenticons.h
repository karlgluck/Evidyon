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
#ifndef __EVIDYONGUIENCHANTMENTICONS_H__
#define __EVIDYONGUIENCHANTMENTICONS_H__
#pragma once


#include "common/gui/guicomponent.h"
#include "common/gui/guicanvasmatrix.h"
#include "common/gui/guipictureelement.h"
#include "shared/magic/buff.h"
#include "shared/magic/buffmask.h"


namespace Evidyon {

// Displays icons for the various enchantments
// It might be cool to add tooltips for the pictures so that
// players can see what they do
class EvidyonGUIEnchantmentIcons : public GUIComponent {
public:
  EvidyonGUIEnchantmentIcons();

  // Sets up the location of the canvases for this display.  The
  // icons will show up just underneath where the chat bar
  // is rendered.
  void init(GUICanvas* primary_canvas, GUISize hp_mp_xp_size);

  // Syncs the icon display to which enchantments are
  // currently active on the client's avatar.  This should
  // be called when no elements are registered.
  void update(const Magic::BuffMask* buff_mask);

public:
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  // Internal method to get the source area of the GUI texture
  // that contains the icon for the given enchantment type.
  void getIconSrcRect(Magic::Buff buff, GUIRect* rect);

private:
  GUICanvasMatrix<1, Magic::NUMBER_OF_BUFFS> canvas_matrix_;
  GUIPictureElement icons_[Magic::NUMBER_OF_BUFFS];
};

}

#endif