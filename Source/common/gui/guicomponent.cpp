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
#include "guipoint.h"
#include "guisize.h"
#include "guirect.h"
#include "guicanvas.h"
#include <d3dx9.h>
#include <map>
#include "guielement.h"
#include "guivertex.h"
#include "guiindex.h"
#include "guitexture.h"
#include "guikeyboard.h"
#include "guimouse.h"
#include "guicomponent.h"

GUIComponent::~GUIComponent() {
}

void GUIComponent::setClippingArea(const GUIRect* area) {
}

void GUIComponent::registerElements(GUILayer* layer) {
}

void GUIComponent::unregisterElements(GUILayer* layer) {
}

GUIMouseSentinel* GUIComponent::getMouseSentinel() {
  return NULL;
}

GUIKeyboardSentinel* GUIComponent::getKeyboardSentinel() {
  return NULL;
}

void GUIComponent::removeFromInputChain() {
  {
  GUIMouseSentinel* mouse = getMouseSentinel();
  if (mouse) mouse->removeFromMouseSentinelChain();
  }
  {
  GUIKeyboardSentinel* keyboard = getKeyboardSentinel();
  if (keyboard) keyboard->removeFromKeyboardSentinelChain();
  }
}

void GUIComponent::setParentSentinels(GUIMouseSentinel* parent_mouse,
                    GUIKeyboardSentinel* parent_keyboard) {
  {
  GUIMouseSentinel* mouse = getMouseSentinel();
  if (mouse) mouse->setParentMouseSentinel(parent_mouse);
  }
  {
  GUIKeyboardSentinel* keyboard = getKeyboardSentinel();
  if (keyboard) keyboard->setParentKeyboardSentinel(parent_keyboard);
  }
}