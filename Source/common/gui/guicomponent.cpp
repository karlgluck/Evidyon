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