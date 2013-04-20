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
#ifndef __EVIDYONGUISCROLLABLEBUTTONLIST_H__
#define __EVIDYONGUISCROLLABLEBUTTONLIST_H__
#pragma once



#include "evidyonguiscrollablelist.h"
#include "evidyonguibutton.h"


namespace Evidyon {



  
//----[  EvidyonGUIScrollableButtonList  ]-------------------------------------
class EvidyonGUIScrollableButtonList : public GUIComponent {
  class Button : public EvidyonGUIButton {
  public:
    Button(EvidyonGUIScrollableButtonList* owner, int id);
    int getID() const;

    // Interprets mouse over and click messages, but always
    // passes back up to EvidyonGUIButton.
    virtual void onStateChange(GUIButton::StateChange stateChange);

  private:
    int id_;
    EvidyonGUIScrollableButtonList* owner_;
  };

public:
  EvidyonGUIScrollableButtonList();
  virtual ~EvidyonGUIScrollableButtonList();

  // Changes how large the buttons are
  void setLineHeight(int pixels);

  // Either obtains a currently existing button or creates a new button
  // and returns that one.
  // The visual properties of the buttons (text and font) must be entirely
  // managed by the class using this one.
  EvidyonGUIButton* button(int id);

  // Erases all buttons.  Be sure to call unregisterElements before this
  // method!
  void clear();

  // Returns 'true' if the mouse is hovering over a button, and passes
  // the ID of that entry in the parameter.
  bool getMouseOverButton(int* id);

  // Returns 'true' if the mouse clicked a button, and passes
  // the ID of that entry in the parameter.  This method will
  // only return 'true' once per button click.
  bool getClickedButton(int* id);

  // Controls the list location
  void setScrollPosition(float position);
  float getScrollPosition();
  void scrollToLine(int line, int pixel_offset);
  //void getLineScrollLocation(int* line, int* pixel_offset);

  void setBackgroundColor(D3DCOLOR color);

public:
  virtual GUIMouseSentinel* getMouseSentinel();
  virtual GUIKeyboardSentinel* getKeyboardSentinel();
  virtual void registerElements(GUILayer* layer);
  virtual void unregisterElements(GUILayer* layer);

private:
  Button* mouse_over_button_;
  Button* mouse_clicked_button_;
  EvidyonGUIScrollableList list_;
};

}



#endif
