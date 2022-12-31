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