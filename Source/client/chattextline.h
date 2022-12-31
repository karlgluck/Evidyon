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
#ifndef __CHATTEXTLINE_H__
#define __CHATTEXTLINE_H__
#pragma once


#include "evidyonguieditabletextline.h"



//----[  ChatTextLine  ]-------------------------------------------------------
class ChatTextLine : public EvidyonGUIEditableTextLine {
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(ChatTextLine* owner);
    void enable(bool enabled) { enabled_ = enabled; }
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);
    virtual bool onTypedCharacter(char character, GUIKeyboardInputSource* keyboard);

  private:
    bool enabled_;
    bool watching_;
    ChatTextLine* owner_;
  };

  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(ChatTextLine* owner);
    void enable(bool enabled) { enabled_ = enabled; }
    virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);
  private:
    bool enabled_;
    ChatTextLine* owner_;
  };

public:
  ChatTextLine();
  bool create(GUIFont* font,
        GUIMouseSentinel* root_mouse_sentinel,
        GUIKeyboardSentinel* root_keyboard_sentinel,
        GUISize hpMpXpSize);
  void destroy();
  void show(GUILayer* layer);

  void update();

protected:
  KeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;
  bool exit_;
  bool send_;
  GUILayer* layer_;
};




#endif
