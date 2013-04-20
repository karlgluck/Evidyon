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
#include "stdafx.h"
#include "evidyonguidefs.h"
#include "evidyonguisounds.h" // for playing button-click sound




//-----------------------------------------------------------------------------
// Name: EvidyonGUIButton
// Desc: Initializes this class
//-----------------------------------------------------------------------------
EvidyonGUIButton::EvidyonGUIButton() {
  myBackgroundCanvas.setSourceCanvas(getCanvas());
  myBackgroundCanvas.alignXCenter(0);
  myBackgroundCanvas.alignYCenter(0);
  myBackgroundCanvas.relativeWidth(0);
  myBackgroundCanvas.relativeHeight(0);
  myBackgroundCanvas.relativeDepth(+0.01f);

  myText.setClippingArea(myBackgroundCanvas.getArea());

  myBackground.setSourceRegion(&EvidyonGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
                                EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                EvidyonGUIDefs::TEXTURE_SIZE);
  myBackground.setSourceCanvas(&myBackgroundCanvas);


  onStateChange(GUIButton::STATECHANGE_MOUSEOVER);
}




//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font this button will use to render its text
//-----------------------------------------------------------------------------
void EvidyonGUIButton::setFont(GUIFont* font)
{
    myText.setFont(font);
}



//-----------------------------------------------------------------------------
// Name: setText
// Desc: Sets that this button will display
//-----------------------------------------------------------------------------
void EvidyonGUIButton::setText(const char* text)
{
    myText.setText(text ? text : "");
    myText.setTextColor(EvidyonGUIDefs::BUTTON_COLOR_NEUTRAL);
//    onStateChange(GUIButton::STATECHANGE_ENABLED);
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this button's elements with the layer so they can be drawn
//-----------------------------------------------------------------------------
void EvidyonGUIButton::registerElements(GUILayer* layer)
{
    myBackground.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    onStateChange(GUIButton::STATECHANGE_ENABLED);
    layer->registerElement(&myBackground);
    GUIButton::registerElements(layer);
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Removes this button's elements from the layer so they aren't drawn anymore
//-----------------------------------------------------------------------------
void EvidyonGUIButton::unregisterElements(GUILayer* layer) {
    layer->unregisterElement(&myBackground);
    GUIButton::unregisterElements(layer);
}

void EvidyonGUIButton::setClippingArea(const GUIRect* area) {
  myBackground.setClippingArea(area);
  GUIButton::setClippingArea(area);
}


//-----------------------------------------------------------------------------
// Name: onStateChange
// Desc: 
//-----------------------------------------------------------------------------
void EvidyonGUIButton::onStateChange(GUIButton::StateChange stateChange) {
  switch(stateChange) {
      case GUIButton::STATECHANGE_RESET: {
        getMouseSentinel()->releaseMouseFocus();
        myText.setTextColor(EvidyonGUIDefs::BUTTON_COLOR_NEUTRAL);
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_DISABLED: {
        myText.setTextColor(EvidyonGUIDefs::BUTTON_COLOR_DISABLED);
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_ENABLED:
        myText.setTextColor(EvidyonGUIDefs::BUTTON_COLOR_NEUTRAL);

      case GUIButton::STATECHANGE_MOUSEOUT: {
        myBackground.setSourceRegion(
           &EvidyonGUIDefs::BUTTON_BACKGROUND_NEUTRAL,
            EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
            EvidyonGUIDefs::TEXTURE_SIZE);
      } break;

      case GUIButton::STATECHANGE_MOUSEOVER: {
        myBackground.setSourceRegion(&EvidyonGUIDefs::BUTTON_BACKGROUND_HOVER,
                                       EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                       EvidyonGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_MOUSEOVERANDDOWN: {
        myBackground.setSourceRegion(&EvidyonGUIDefs::BUTTON_BACKGROUND_MOUSEDOWN,
                                      EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                      EvidyonGUIDefs::TEXTURE_SIZE);

      } break;

      case GUIButton::STATECHANGE_PUSHED: {
        Evidyon::EvidyonGUISounds::buttonClick();
      } break;
    }
}

