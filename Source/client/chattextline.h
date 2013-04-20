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
