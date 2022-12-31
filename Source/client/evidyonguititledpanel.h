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
#ifndef __EVIDYONGUITITLEDPANEL_H__
#define __EVIDYONGUITITLEDPANEL_H__
#pragma once


#include "common/gui/guicomponent.h"
#include "common/gui/guiborderedfillelement.h"
#include "common/gui/guitextlabelelement.h"

class GUIFont;

/**
* Draws a panel with a title
*/
class EvidyonGUITitledPanel : public GUIComponent
{
public:

  /**
   * Initializes this class
   */
  EvidyonGUITitledPanel();

  /**
   * Changes the font this panel will use to render its text
   */
  void setFont(GUIFont* font);

  /**
   * Sets the label for this panel
   */
  void setText(const char* text);

  /**
   * Registers this panel's visible elements with the given layer
   */
  void registerElements(GUILayer* layer);

  /**
   * Unregisters this panel's visible elements
   */
  void unregisterElements(GUILayer* layer);

  void setColor(D3DCOLOR color);


protected:

  /// Title's canvas
  GUICanvas myTitleCanvas;

  /// Title background
  GUIBorderedFillElement myTitleBackground;

  /// Title text renderer
  GUITextLabelElement myTitleText;

  /// Background for the whole panel
  GUIBorderedFillElement myPanelBackground;
};


#endif