//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008 - 2013 Karl Gluck                                       //
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
#ifndef __EVIDYONGUIACTIONBUTTONLIST_H__
#define __EVIDYONGUIACTIONBUTTONLIST_H__
#pragma once


#include "evidyonguiscrollablebuttonlist.h"
#include "common/gui/guimodule.h"

namespace Evidyon {
class ActionTargeter;
class EvidyonGUITooltipBox;


// the purpose of this class is to make it *really* easy to create
// a list with arbitrary contents that allows per-item tooltips
// and will generate an action when a list member is clicked.
class EvidyonGUIActionButtonList : public GUIModule {
public:

  // Resets internal values
  EvidyonGUIActionButtonList();

  // Gets the positioning canvas for this list.
  GUICanvas* getListCanvas();

  // Gets the font used on the face of buttons
  GUIFont* getButtonFont();

  // Checks this list for changes.  If a button is clicked, this
  // method calls onButtonClicked.  When the mouse hovers over
  // a button, it sets up the tooltip and calls addTooltipContent
  // to fill it out.
  void update(ActionTargeter* action_targeter);

  EvidyonGUITooltipBox* getTooltip();

protected:

  // Initializes the button list.  This should be invoked first
  // by the derived class when it is set up.
  void createActionButtonList(
      GUIFont* button_font,
      EvidyonGUITooltipBox* tooltip);

  // Shuts down the list.  Should be called last when the
  // class is cleaned up.
  void destroyActionButtonList();

  bool getMouseOverButton(int* button_id);
  bool getClickedButton(int* button_id);

  // Removes all of the buttons from the button list.
  // *** THIS METHOD DOES NOT UNREGISTER ELEMENTS ***
  // It is important that this method only be called when the layer
  // is hidden.  The GUIModule makes this easy:
  //    GUILayer* layer = hide();
  //    ...
  //    clearButtons();
  //    ...
  //    show(layer);
  void clearButtons();

  // Creates a button in the list.  If this ID already exists, it
  // is overwritten.  The 'text' parameter is what will be displayed
  // in the list itself.  The button is set up for rendering automatically
  // and doesn't need any further formatting (its font is set, it is
  // enabled and it is properly aligned).  The return value can be
  // safely ignored in most cases.
  // Like clearButtons, this should only be called when the list is
  // invisible.
  EvidyonGUIButton* addButton(int id, const char* text);


protected:

  // Called to notify the derived class that the given button
  // was pressed.
  virtual void onButtonClicked(
      int button_id,
      ActionTargeter* action_targeter);

  // Invoked when the mouse hovers over a button.  Should be
  // implemented by the derived class to add info to the tooltip.
  // Be sure to set the tooltip's size!
  // If this method returns 'false', the tooltip won't be shown.
  virtual bool updateTooltipContent(int button_id, EvidyonGUITooltipBox* tooltip);
  virtual void addTooltipContent(EvidyonGUITooltipBox* tooltip);


private:
  GUIFont* button_font_;
  EvidyonGUITooltipBox* tooltip_;
  EvidyonGUIScrollableButtonList list_;
};


}



#endif