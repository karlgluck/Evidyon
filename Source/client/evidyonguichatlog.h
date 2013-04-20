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
#ifndef __CLIENT_EVIDYONGUICHATLOG_H__
#define __CLIENT_EVIDYONGUICHATLOG_H__
#pragma once

#include "evidyonguiscrollablelist.h"
#include "shared/actor/actoralignment.h"
#include "common/gui/guimodule.h"

class GUIFont;

namespace Evidyon {
namespace Client {


  
//----[  EvidyonGUIChatLog  ]--------------------------------------------------
class EvidyonGUIChatLog : public GUIModule {
public:
  EvidyonGUIChatLog();
  void create(GUICanvas* source_canvas, GUIFont* font);
  void destroy();

  // Displays the layer and gets mouse focus
  void show(GUILayer* layer);
  void hide();

  void addChatText(Actor::ActorAlignment alignment,
                   const std::string& actor_name,
                   const std::string& text);

  void addGuildText(const std::string& actor_name,
                    const std::string& text);

  void addGlobalText(const std::string& actor_name,
                     const std::string& text);

public:
  // Overrides these methods to disable passing messages to all other components
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Catch the ESCAPE key so that pressing it only hits the cancel button
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);


private:
  GUIFont* font_;
  EvidyonGUIScrollableList list_;
  int next_line_id_;
};



}
}

#endif