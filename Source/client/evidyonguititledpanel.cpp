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



//-----------------------------------------------------------------------------
// Name: EvidyonGUITitledPanel
// Desc: Initializes this class
//-----------------------------------------------------------------------------
EvidyonGUITitledPanel::EvidyonGUITitledPanel()
{
  GUICanvas* canvas = getCanvas();
  canvas->relativeWidth(0);
  canvas->absoluteHeight(32);
  canvas->alignXCenter(0);
  canvas->alignYTopOutside(0);
  canvas->relativeDepth(0.0f);

  {
      myTitleBackground.setSourceCanvas(&myTitleCanvas);
      myTitleBackground.setSourceRegion(&EvidyonGUIDefs::WINDOW_TITLE_BACKGROUND,
                                         EvidyonGUIDefs::WINDOW_TITLE_BACKGROUND_BORDER,
                                         EvidyonGUIDefs::TEXTURE_SIZE);
  }

  {
      myPanelBackground.setSourceCanvas(canvas);
      myPanelBackground.setSourceRegion(&EvidyonGUIDefs::WINDOW_BACKGROUND,
                                          EvidyonGUIDefs::WINDOW_BACKGROUND_BORDER,
                                          EvidyonGUIDefs::TEXTURE_SIZE);
  }

  {
      myTitleCanvas.setSourceCanvas(canvas);
      myTitleCanvas.alignXCenter(0);
      myTitleCanvas.relativeDepth(0.0f);
      myTitleCanvas.relativeWidth(0);
      myTitleCanvas.absoluteHeight(30);
      myTitleCanvas.alignYTopOutside(0);
      myTitleText.setSourceCanvas(&myTitleCanvas);
      myTitleText.setTextColor(EvidyonGUIDefs::WINDOW_TITLE_TEXT_COLOR);
      myTitleText.setAlignment(GUIALIGN_CENTER);
  }

}





//-----------------------------------------------------------------------------
// Name: setFont
// Desc: Changes the font this panel will use to render its text
//-----------------------------------------------------------------------------
void EvidyonGUITitledPanel::setFont(GUIFont* font)
{
    myTitleText.setFont(font);
}



//-----------------------------------------------------------------------------
// Name: setText
// Desc: Sets the label for this panel
//-----------------------------------------------------------------------------
void EvidyonGUITitledPanel::setText(const char* text)
{
    myTitleText.setText(text);
}



//-----------------------------------------------------------------------------
// Name: registerElements
// Desc: Registers this panel's visible elements with the given layer
//-----------------------------------------------------------------------------
void EvidyonGUITitledPanel::registerElements(GUILayer* layer)
{
    myPanelBackground.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    myTitleBackground.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);
    layer->registerElement(&myPanelBackground);
    layer->registerElement(&myTitleBackground);
    layer->registerElement(&myTitleText);
}



//-----------------------------------------------------------------------------
// Name: unregisterElements
// Desc: Unregisters this panel's visible elements
//-----------------------------------------------------------------------------
void EvidyonGUITitledPanel::unregisterElements(GUILayer* layer)
{
    layer->unregisterElement(&myPanelBackground);
    layer->unregisterElement(&myTitleBackground);
    layer->unregisterElement(&myTitleText);
}



//----[  setColor  ]-----------------------------------------------------------
void EvidyonGUITitledPanel::setColor(D3DCOLOR color) {
  myPanelBackground.setColor(color);
  myTitleBackground.setColor(color);
}

