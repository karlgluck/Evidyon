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
#ifndef __GUICOMPONENT_H__
#define __GUICOMPONENT_H__
#pragma once



#include "guicanvas.h"


struct GUIRect;
class GUILayer;
class GUIMouseSentinel;
class GUIKeyboardSentinel;



class GUIComponent {
public:
  virtual ~GUIComponent();
  virtual void setClippingArea(const GUIRect* area);
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);
  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();
  inline GUICanvas* getCanvas();

  // Removes the mouse and/or keyboard sentinel from the input chain
  void removeFromInputChain();

  // Sets the parent of the parent mouse/keyboard sentinels--if they exist
  void setParentSentinels(GUIMouseSentinel* parent_mouse,
              GUIKeyboardSentinel* parent_keyboard);

private:
  GUICanvas component_canvas_;
};


GUICanvas* GUIComponent::getCanvas() {
  return &component_canvas_;
}



#endif