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
#ifndef __EVIDYONGUIBUTTON_H__
#define __EVIDYONGUIBUTTON_H__
#pragma once


#include "common/gui/guicanvas.h"
#include "common/gui/guiborderedfillelement.h"
#include "common/gui/guibutton.h"

class GUIFont;
class GUILayer;
struct GUIRect;


//----[  EvidyonGUIButton  ]---------------------------------------------------
// Handles the rendering and behavior of an Evidyon-specific button
class EvidyonGUIButton : public GUIButton
{
public:

  // Initializes this class
  EvidyonGUIButton();

  // Changes the font this button will use to render its text
  void setFont(GUIFont* font);

  // Sets that this button will display
  void setText(const char* text);

  // Registers this button's elements with the layer so they can be drawn
  virtual void registerElements(GUILayer* layer);

  // Removes this button's elements from the layer so they aren't drawn anymore
  virtual void unregisterElements(GUILayer* layer);

  virtual void setClippingArea(const GUIRect* area);

protected:

  // This method is called when button's state changes
  virtual void onStateChange(GUIButton::StateChange stateChange);


protected:

  /// The canvas for the background of this button
  /// The class that renders the background of this button
  GUICanvas myBackgroundCanvas;
  GUIBorderedFillElement myBackground;
};


#endif