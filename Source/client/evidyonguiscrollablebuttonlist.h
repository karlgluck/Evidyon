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
