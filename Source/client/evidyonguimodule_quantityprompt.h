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
#ifndef __EVIDYONGUIMODULE_QUANTITYPROMPT_H__
#define __EVIDYONGUIMODULE_QUANTITYPROMPT_H__
#pragma once

#include "common/gui/guimodule.h"
#include "common/gui/guitextlabelcomponent.h"
#include "evidyonguititledpanel.h"
#include "evidyonguieditabletextline.h"
#include "evidyonguibutton.h"
#include "evidyonguititledpanel.h"

struct ClientItemDescription;

namespace Evidyon {


class EvidyonGUIModule_QuantityPrompt : public GUIModule {
public:

  // Initializes this module
  void create(GUICanvas* source_canvas,
              GUIFont* prompt_font);

  // Cleans up the module
  void destroy();

  // Sets up the prompt text and gives this module focus
  void initialize(const char* prompt_text,
                  const char* ok_text,
                  const char* cancel_text);

  void setOKText(const char* ok_text);
  void enableOK(bool enable);

  bool pollOK();
  bool pollCancel();
  unsigned int getValue();


public:
  // Overrides these methods to disable passing messages to all other components
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Catch the ESCAPE key so that pressing it only hits the cancel button
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);


private:
  EvidyonGUIButton ok_, cancel_;
  EvidyonGUITitledPanel panel_;
  GUITextLabelComponent prompt_text_;
  EvidyonGUIEditableTextLine input_;
};


}


#endif