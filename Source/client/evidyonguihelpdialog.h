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
#ifndef __CLIENT_EVIDYONGUIHELPDIALOG_H__
#define __CLIENT_EVIDYONGUIHELPDIALOG_H__
#pragma once

#include "evidyonguiscrollablelist.h"
#include "shared/actor/actoralignment.h"
#include "common/gui/guimodule.h"
#include "evidyonguititledpanel.h"
#include "evidyonguibutton.h"
#include "common/gui/guitextlabelcomponent.h"

class GUIFont;

namespace Evidyon {
namespace Client {


//----[  EvidyonGUIHelpDialog  ]-----------------------------------------------
class EvidyonGUIHelpDialog : public GUIModule {
public:
  EvidyonGUIHelpDialog();
  void create(GUICanvas* source_canvas, GUIFont* font);
  void destroy();

  // Displays the layer and gets mouse focus
  void show(GUILayer* layer);
  void hide();

public:
  // Overrides these methods to disable passing messages to all other components
  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Catch the ESCAPE key so that pressing it only hits the cancel button
  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);


private:
  EvidyonGUITitledPanel panel_;
  GUITextLabelComponent help_text_;
  EvidyonGUIButton ok_button_;
};

}
}

#endif