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
#ifndef __EVIDYONGUISTATDISPLAY_H__
#define __EVIDYONGUISTATDISPLAY_H__
#pragma once



class UserEventQueue;

#include "evidyonguititledpanel.h"
#include "common/gui/guitextlabelcomponent.h"
#include "shared/avatar/evidyon_avatar.h"
#include "shared/avatar/avatarrace.h"
#include "shared/avatar/avatarclass.h"

namespace Evidyon {
namespace Network {
namespace Packet {
struct NetMsg_TellYourAvatarStats_Data;
}
}
}

//----[  EvidyonGUIStatDisplay  ]----------------------------------------------
class EvidyonGUIStatDisplay
{
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(EvidyonGUIStatDisplay* owner);
    void enable(bool enabled);
    bool isEnabled() { return enabled_; }
    virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

  private:
    EvidyonGUIStatDisplay* owner_;
    bool enabled_;
  };

public:
  EvidyonGUIStatDisplay();
  bool create(GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
              GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel,
              UserEventQueue* user_action_queue);
  void destroy();
  //void update(unsigned char level,
  //            unsigned char abilities[5],
  //            unsigned char availableAbilityPoints,
  //            int hp,
  //            int hpMax,
  //            int mp,
  //            int mpMax,
  //            int exp,
  //            int nextLevelExp,
  //            int defense,
  //            Evidyon::Avatar::AvatarRace avatar_race,
  //            Evidyon::Avatar::AvatarClass avatar_class,
  //            int alignment_value,
  //            const char* alignment_class_text,
  //            D3DCOLOR alignment_class_color);
  void update(const Evidyon::Network::Packet::NetMsg_TellYourAvatarStats_Data* data,
              const char* alignment_class_text,
              D3DCOLOR alignment_class_color,
              unsigned int gold);
                

  /**
   * Displays the stat screen.  If it hasn't been updated, it will show invalid data.
   */
  void show();

  /**
   * Removes all of this display's items from the screen and mouse/keyboard input chain
   */
  void hideDisplay();

  /**
   * If the finished button was pushed, this method returns true.  Subsequent calls will return false
   * until the user pushes the finished button again.
   */
  bool pollFinishedButton();

  /**
   * If the user clicks a button to add an ability point then this will
   * return 'true' and set 'ability' to the index of that stat
   */
  bool pollAddAbilityPoint(int* ability);

  void updateUserEvents();


protected:

  /**
   * Resets this class
   */
  void zero();


protected:

  /// The target layer for this display
  GUILayer* myRenderingLayer;

  /// The owner mouse sentinel for this display
  GUIMouseSentinel* myParentMouseSentinel;

  /// The panel where the avatar stats are shown
  EvidyonGUITitledPanel myAvatarPanel;

  /// When the user presses this button, they want to leave the screen
  EvidyonGUIButton myFinishedButton;

  /// If there are points available, buttons are shown for the stats instead of text
  GUICanvas myAbilityTextCanvas[5];
  GUITextLabelElement myAbilityText[5];
  EvidyonGUIButton myAbilityButtons[5];

  /// Player stats (level/hp/mp/exp)
  GUICanvas myStatsTextCanvas;
  GUITextLabelElement myStatsText;

  /// Text declaring misc. information about the character
  GUICanvas myDescriptionTextCanvas;
  GUITextLabelElement myDescriptionText;

  /// Whether or not to prompt the user to assign stat points
  bool myStatPointsAreAvailableToBeAssigned;

  UserEventQueue* user_action_queue_;
  KeyboardSentinel keyboard_sentinel_;

  GUITextLabelComponent race_class_text_;

  GUITextLabelComponent alignment_label_;
  GUICanvas alignment_background_canvas_;
  GUIBorderedFillElement alignment_background_;
  GUICanvas alignment_bar_canvas_;
  GUIBorderedFillElement alignment_bar_;
  GUITextLabelComponent alignment_text_;

  GUITextLabelComponent gold_label_;
};


#endif