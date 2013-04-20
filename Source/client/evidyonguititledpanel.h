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