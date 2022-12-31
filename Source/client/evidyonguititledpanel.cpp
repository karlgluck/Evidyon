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

