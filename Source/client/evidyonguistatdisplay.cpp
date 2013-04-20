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
#include "stdafx.h"
#include "evidyonguistatdisplay.h"
#include "useractionqueue.h"
#include "evidyonguidefs.h"

#include "shared/client-server/packets.h" // for NetMsg_TellYourAvatarStats_Data

//-----------------------------------------------------------------------------
EvidyonGUIStatDisplay::KeyboardSentinel::KeyboardSentinel(EvidyonGUIStatDisplay* owner) {
  owner_ = owner;
  enabled_ = false;
}



//-----------------------------------------------------------------------------
void EvidyonGUIStatDisplay::KeyboardSentinel::enable(bool enabled) {
  enabled_ = enabled;
}



//-----------------------------------------------------------------------------
bool EvidyonGUIStatDisplay::KeyboardSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (!enabled_) return false; // don't handle messages unless enabled
  if (GUIKeyboardSentinel::onKeyUp(key, keyboard)) return true; // don't process if the key was caught
  if (key == GUIKEY_ESCAPE) {
    owner_->myFinishedButton.push();
    return true;
  }
  return false;
}






//-----------------------------------------------------------------------------
// Name: EvidyonGUIStatDisplay
// Desc: Initializes this class
//-----------------------------------------------------------------------------
EvidyonGUIStatDisplay::EvidyonGUIStatDisplay() : keyboard_sentinel_(this)
{
    zero();
}



//-----------------------------------------------------------------------------
// Name: create
// Desc: Initializes this display of this stat display on the target rendering layer.  This method
//       doesn't draw anything to the layer until the display method is invoked.
//-----------------------------------------------------------------------------
bool EvidyonGUIStatDisplay::create(GUIFont* windowFont, GUIFont* titleFont, GUILayer* layer,
    GUIMouseSentinel* parentMouseSentinel, GUIKeyboardSentinel* parentKeyboardSentinel, UserEventQueue* user_action_queue)
{
    CONFIRM(windowFont && titleFont && parentMouseSentinel) else return false;

    // Save persistent parameters
    myRenderingLayer = layer;
    myParentMouseSentinel = parentMouseSentinel;
    user_action_queue_ = user_action_queue;
    keyboard_sentinel_.setParentKeyboardSentinel(parentKeyboardSentinel);

    // Set up all of the panel canvases
    myAvatarPanel.getCanvas()->setSourceCanvas(
      layer->getPrimaryCanvas());

    // Set the parameters for each of the panels
    myAvatarPanel.setFont(titleFont);
    myAvatarPanel.setColor(0xA0FFFFFF);

    for (int i = 0; i < 5; ++i) {
      myAbilityText[i].setFont(windowFont);
      myAbilityText[i].setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
      myAbilityButtons[i].setFont(windowFont);

    }

    myStatsText.setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    myStatsText.setFont(titleFont);

    // Set the parameters for the buttons
    myFinishedButton.setFont(titleFont);


    myDescriptionText.setFont(windowFont);
    myDescriptionText.setTextColor(EvidyonGUIDefs::CONTENT_COMPONENT_TEXT_COLOR);
    alignment_text_.textLabel()->setFont(titleFont);
    alignment_label_.textLabel()->setFont(titleFont);
    race_class_text_.textLabel()->setFont(titleFont);
    gold_label_.textLabel()->setFont(titleFont);

    // Success
    return true;
}



//-----------------------------------------------------------------------------
// Name: destroy
// Desc: Erases all of the members of this screen, and removes its components from the
//       display layer.
//-----------------------------------------------------------------------------
void EvidyonGUIStatDisplay::destroy()
{
    hideDisplay();
    keyboard_sentinel_.removeFromKeyboardSentinelChain();

    zero();
}


//
////-----------------------------------------------------------------------------
//// Name: display
//// Desc: Sets up this display to show the given avatar
////-----------------------------------------------------------------------------
//void EvidyonGUIStatDisplay::update(unsigned char level,
//                                   unsigned char abilities[5],
//                                   unsigned char availableAbilityPoints,
//                                   int hp, int hpMax,
//                                   int mp, int mpMax,
//                                   int exp,
//                                   int nextLevelExp,
//                                   int defense,
//                                   Evidyon::Avatar::AvatarRace avatar_race,
//                                   Evidyon::Avatar::AvatarClass avatar_class,
//                                   int alignment_value,
//                                   const char* alignment_class_text,
//                                   D3DCOLOR alignment_class_color)
//{
//  myStatPointsAreAvailableToBeAssigned = availableAbilityPoints > 0;
//
//  if (availableAbilityPoints) {
//      char text[128];
//      sprintf_s(text, sizeof(text), "%lu stat point%s available", availableAbilityPoints, availableAbilityPoints != 1 ? "s are" : " is");
//      myDescriptionText.setText(text);
//  }
//  else
//      myDescriptionText.setText("");
//
//  // Build attribute text
//  for (int i = 0; i < 5; ++i)
//  {
//      const char* mask;
//      switch(i)
//      {
//          case 0: mask = "%lu  strength"; break;
//          case 1: mask = "%lu  agility"; break;
//          case 2: mask = "%lu  constitution"; break;
//          case 3: mask = "%lu  intelligence"; break;
//          case 4: mask = "%lu  wisdom"; break;
//      }
//
//      char abilityText[128];
//      sprintf_s(abilityText, sizeof(abilityText), mask, abilities[i]);
//
//      myAbilityText[i].setText(abilityText);
//
//      if (myStatPointsAreAvailableToBeAssigned)
//      {
//          myAbilityButtons[i].enable();
//          myAbilityButtons[i].resetPushed();
//      }
//  }
//
//  // Build the stat text
//  char statText[512];
//  sprintf_s(statText, sizeof(statText), "Level:   %i\n\n"\
//                                         "HP:      %i / %i\n"\
//                                         "MP:      %i / %i\n"\
//                                         "EXP:     %i / %i\n\n"\
//                                         "defense: %i",
//             level, hp, hpMax, mp, mpMax, exp, nextLevelExp, defense);
//  myStatsText.setText(statText);
//
//  // Reset button states
//  myFinishedButton.enable();
//  myFinishedButton.resetPushed();
//
//  alignment_bar_.setColor(alignment_class_color);
//  alignment_bar_canvas_.alignXCenter(alignment_value*1.5);
//  alignment_text_.textLabel()->setText(alignment_class_text);
//  alignment_text_.textLabel()->setTextColor(alignment_class_color);
//
//  char temp[128];
//  sprintf_s(temp, 128, "%s %s",
//            Evidyon::Avatar::AvatarRaceString(avatar_race),
//            Evidyon::Avatar::AvatarClassString(avatar_class));
//  race_class_text_.textLabel()->setText(race_class_string);
//
//}
//



//----[  update  ]-------------------------------------------------------------
void EvidyonGUIStatDisplay::update(
    const Evidyon::Network::Packet::NetMsg_TellYourAvatarStats_Data* data,
    const char* alignment_class_text,
    D3DCOLOR alignment_class_color,
    unsigned int gold) {
  myStatPointsAreAvailableToBeAssigned = data->available_attribute_points > 0;

  if (myStatPointsAreAvailableToBeAssigned) {
      char text[128];
      sprintf_s(text, sizeof(text), "%lu stat point%s available",
        data->available_attribute_points,
        data->available_attribute_points != 1 ? "s are" : " is");
      myDescriptionText.setText(text);
  }
  else
      myDescriptionText.setText("");

  // Build attribute text
  for (int i = 0; i < 5; ++i)
  {
      const char* mask;
      switch(i)
      {
          case 0: mask = "%lu  strength"; break;
          case 1: mask = "%lu  agility"; break;
          case 2: mask = "%lu  constitution"; break;
          case 3: mask = "%lu  intelligence"; break;
          case 4: mask = "%lu  wisdom"; break;
      }

      char abilityText[128];
      sprintf_s(
        abilityText,
        sizeof(abilityText),
        mask,
        unsigned int(data->attribute_values[i]));

      myAbilityText[i].setText(abilityText);

      if (myStatPointsAreAvailableToBeAssigned) {
        myAbilityButtons[i].enable();
        myAbilityButtons[i].resetPushed();
      }
  }

  // Build the stat text
  char hp_text[128], mp_text[128], xp_text[128], penalty_xp_text[128];
  sprintf_s(hp_text, 128, "\n   +%i%% max from guild", data->guild_max_hp);
  sprintf_s(mp_text, 128, "\n   +%i%% max from guild", data->guild_max_mp);
  sprintf_s(xp_text, 128, "\n   +%i%% xp/kill from guild", data->guild_xp);
  sprintf_s(penalty_xp_text, 128, "\n   -%lld penalty xp", data->penalty_xp);
  char statText[512];
  sprintf_s(
    statText,
    sizeof(statText),
    "Level:   %i\n\n"\
    "HP:      %i / %i\n  regenerate +%i%s\n"\
    "MP:      %i / %i\n  regenerate +%i%s\n"\
    "EXP:     %lld / %lld%s%s\n"\
    "defense: %i",
    int(data->level),
    int(data->hp),
    int(data->max_hp),
    int(data->hp_regen),
    data->guild_max_hp != 0 ? hp_text : "\n",
    int(data->mp),
    int(data->max_mp),
    int(data->mp_regen),
    data->guild_max_mp != 0 ? mp_text : "\n",
    data->current_xp,
    data->next_level_xp,
    data->guild_xp != 0 ? xp_text : "\n",
    data->penalty_xp != 0 ? penalty_xp_text : "\n",
    int(data->defense));
  myStatsText.setText(statText);

  // Reset button states
  myFinishedButton.enable();
  myFinishedButton.resetPushed();

  alignment_bar_.setColor(alignment_class_color);
  alignment_bar_canvas_.alignXCenter(int(data->alignment)*1.5);
  alignment_text_.textLabel()->setText(alignment_class_text);
  alignment_text_.textLabel()->setTextColor(alignment_class_color);

  char temp[128];
  sprintf_s(temp, 128, "%s %s %s",
    Evidyon::Avatar::AvatarGenderString(Evidyon::Avatar::AvatarGender(data->gender)),
    Evidyon::Avatar::AvatarRaceString(Evidyon::Avatar::AvatarRace(data->race)),
    Evidyon::Avatar::AvatarClassString(Evidyon::Avatar::AvatarClass(data->class_)));
  race_class_text_.textLabel()->setText(temp);

  sprintf_s(temp, 128, "%lu Gold", gold);
  gold_label_.textLabel()->setText(temp);
}

//-----------------------------------------------------------------------------
// Name: show
// Desc: Displays the stat screen.  If it hasn't been updated, it will show invalid data.
//-----------------------------------------------------------------------------
void EvidyonGUIStatDisplay::show()
{
    // Remove all items from the display
    hideDisplay();
    
    keyboard_sentinel_.enable(true);

    // Make sure this class is valid
    CONFIRM(myRenderingLayer) else return;

    for (int i = 0; i < 5; ++i)
        myRenderingLayer->registerElement(&myAbilityText[i]);

    if (myStatPointsAreAvailableToBeAssigned)
    {
        for (int i = 0; i < 5; ++i)
        {
            myAbilityButtons[i].resetPushed();
            myAbilityButtons[i].registerElements(myRenderingLayer);
            myAbilityButtons[i].getMouseSentinel()->setParentMouseSentinel(myParentMouseSentinel);
        }
    }


    myAvatarPanel.registerElements(myRenderingLayer);
    myRenderingLayer->registerElement(&myStatsText);
    myRenderingLayer->registerElement(&myDescriptionText);
    myRenderingLayer->registerElement(&alignment_background_);
    myRenderingLayer->registerElement(&alignment_bar_);
    alignment_text_.registerElements(myRenderingLayer);
    alignment_label_.registerElements(myRenderingLayer);
    race_class_text_.registerElements(myRenderingLayer);
    gold_label_.registerElements(myRenderingLayer);
}



//-----------------------------------------------------------------------------
// Name: hideDisplay
// Desc: Removes all of this display's items from the screen and mouse/keyboard input chain
//-----------------------------------------------------------------------------
void EvidyonGUIStatDisplay::hideDisplay() {
    gold_label_.unregisterElements(myRenderingLayer);
    myRenderingLayer->unregisterElement(&alignment_background_);
    myRenderingLayer->unregisterElement(&alignment_bar_);
    alignment_text_.unregisterElements(myRenderingLayer);
    alignment_label_.unregisterElements(myRenderingLayer);
    race_class_text_.unregisterElements(myRenderingLayer);

    keyboard_sentinel_.enable(false);
    if (!myRenderingLayer) return;

    for (int i = 0; i < 5; ++i)
    {
        myRenderingLayer->unregisterElement(&myAbilityText[i]);
        myAbilityButtons[i].unregisterElements(myRenderingLayer);
        myAbilityButtons[i].getMouseSentinel()->removeFromMouseSentinelChain();
    }

    myAvatarPanel.unregisterElements(myRenderingLayer);
    myRenderingLayer->unregisterElement(&myStatsText);
    myRenderingLayer->unregisterElement(&myDescriptionText);
    myFinishedButton.unregisterElements(myRenderingLayer);
    myFinishedButton.getMouseSentinel()->removeFromMouseSentinelChain();
}



//-----------------------------------------------------------------------------
// Name: pollFinishedButton
// Desc: If the finished button was pushed, this method returns true.  Subsequent calls will return false
//       until the user pushes the finished button again.
//-----------------------------------------------------------------------------
bool EvidyonGUIStatDisplay::pollFinishedButton()
{
    bool pushed = myFinishedButton.wasPushed();
    if (pushed) myFinishedButton.resetPushed();
    return pushed;
}





//-----------------------------------------------------------------------------
// Name: pollAddAbilityPoint
// Desc: If the user clicks a button to add an ability point then this will
//       return 'true' and set 'ability' to the index of that stat
//-----------------------------------------------------------------------------
bool EvidyonGUIStatDisplay::pollAddAbilityPoint(int* ability)
{
    for (int i = 0; i < 5; ++i)
    {
        if (myAbilityButtons[i].wasPushed())
        {
            *ability = i;
            myAbilityButtons[i].resetPushed();
            return true;
        }
    }
    return false;
}


void EvidyonGUIStatDisplay::updateUserEvents() {
  int ability;
  if (pollAddAbilityPoint(&ability)) {
    UserEventAddAbilityPoint* add_ability_point = user_action_queue_->addAttributesAddAbilityPoint();
    add_ability_point->ability = ability;
  }
  if (pollFinishedButton()) {
    user_action_queue_->addCloseStatDisplay();
  }
}



//-----------------------------------------------------------------------------
// Name: zero
// Desc: Resets this class
//-----------------------------------------------------------------------------
void EvidyonGUIStatDisplay::zero()
{
    myStatPointsAreAvailableToBeAssigned = false;

    myRenderingLayer = 0;
    myParentMouseSentinel = 0;

    myAvatarPanel.setText("Avatar Stats");
    myAvatarPanel.getCanvas()->absoluteWidth(400);
    myAvatarPanel.getCanvas()->absoluteHeight(350);
    myAvatarPanel.getCanvas()->alignXCenter(0);
    myAvatarPanel.getCanvas()->alignYCenter(0);
    myAvatarPanel.getCanvas()->absoluteDepth(0.5f);

    for (int i = 0; i < 5; ++ i)
    {
        myAbilityTextCanvas[i].absoluteWidth(150);
        myAbilityTextCanvas[i].absoluteHeight(25); // to make centering work
        myAbilityTextCanvas[i].alignXRight(-55);
        myAbilityTextCanvas[i].alignYTop(35 + 35 * i);
        myAbilityTextCanvas[i].relativeDepth(-0.1f);
        myAbilityTextCanvas[i].setSourceCanvas(myAvatarPanel.getCanvas());

        myAbilityText[i].setSourceCanvas(&myAbilityTextCanvas[i]);
        myAbilityText[i].setAlignment(GUIALIGN_RIGHT);

        myAbilityButtons[i].setText("+");
        myAbilityButtons[i].getCanvas()->absoluteWidth(35);
        myAbilityButtons[i].getCanvas()->absoluteHeight(25);
        myAbilityButtons[i].getCanvas()->alignXRight(-15);
        myAbilityButtons[i].getCanvas()->alignYTop(35+35*i);
        myAbilityButtons[i].getCanvas()->relativeDepth(-0.1f);
        myAbilityButtons[i].getCanvas()->setSourceCanvas(myAvatarPanel.getCanvas());
    }

    myStatsTextCanvas.absoluteWidth(190);
    myStatsTextCanvas.relativeHeight(-30);
    myStatsTextCanvas.alignXLeft(+25);
    myStatsTextCanvas.alignYTop(35);
    myStatsTextCanvas.relativeDepth(-0.01f);
    myStatsTextCanvas.setSourceCanvas(myAvatarPanel.getCanvas());

    myStatsText.setSourceCanvas(&myStatsTextCanvas);
    myStatsText.setTextColor(D3DCOLOR_XRGB(0,0,0));
    myStatsText.setAlignment(GUIALIGN_LEFT_TOP);

    myDescriptionTextCanvas.relativeWidth(-35);
    myDescriptionTextCanvas.absoluteHeight(0);
    myDescriptionTextCanvas.alignYTop(20);
    myDescriptionTextCanvas.alignXCenter(0);
    myDescriptionTextCanvas.relativeDepth(-0.1f);
    myDescriptionTextCanvas.setSourceCanvas(myAvatarPanel.getCanvas());

    myDescriptionText.setSourceCanvas(&myDescriptionTextCanvas);
    myDescriptionText.setTextColor(D3DCOLOR_XRGB(0,0,0));
    myDescriptionText.setAlignment(GUIALIGN_RIGHT);

    myFinishedButton.setText("Finished");
    myFinishedButton.getCanvas()->absoluteWidth(200);
    myFinishedButton.getCanvas()->absoluteHeight(35);
    myFinishedButton.getCanvas()->alignXCenter(0);
    myFinishedButton.getCanvas()->alignYBottom(-20);
    myFinishedButton.getCanvas()->relativeDepth(-0.1f);
    myFinishedButton.getCanvas()->setSourceCanvas(myAvatarPanel.getCanvas());

    // generate the alignment background
    alignment_background_canvas_.setSourceCanvas(myAvatarPanel.getCanvas());
    alignment_background_canvas_.alignXCenter(0);
    alignment_background_canvas_.alignYBottom(-80);
    alignment_background_canvas_.relativeDepth(-0.01f);
    alignment_background_canvas_.absoluteWidth(300 + 16);
    alignment_background_canvas_.absoluteHeight(16);
    alignment_background_.setSourceCanvas(&alignment_background_canvas_);
    alignment_background_.setSourceRegion(&EvidyonGUIDefs::WINDOW_TITLE_BACKGROUND,
                                           EvidyonGUIDefs::WINDOW_TITLE_BACKGROUND_BORDER,
                                           EvidyonGUIDefs::TEXTURE_SIZE);
    alignment_background_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);

    // create the slider
    alignment_bar_canvas_.setSourceCanvas(&alignment_background_canvas_);
    alignment_bar_canvas_.relativeDepth(-0.01f);
    alignment_bar_canvas_.relativeHeight(+16);
    alignment_bar_canvas_.absoluteWidth(16);
    alignment_bar_canvas_.alignXCenter(0);
    alignment_bar_canvas_.alignYCenter(0);
    alignment_bar_.setSourceCanvas(&alignment_bar_canvas_);
    alignment_bar_.setSourceRegion(&EvidyonGUIDefs::BUTTON_BACKGROUND_HOVER,
                                    EvidyonGUIDefs::BUTTON_BORDER_PIXELS,
                                    EvidyonGUIDefs::TEXTURE_SIZE);
    alignment_bar_.setTexture(EvidyonGUIDefs::GUI_TEXTURE_INDEX);

    { // put the text below the slider's tab
      alignment_text_.textLabel()->setAlignment(GUIALIGN_CENTER);
      alignment_text_.textLabel()->setText("Alignment");
      GUICanvas* canvas = alignment_text_.getCanvas();
      canvas->setSourceCanvas(&alignment_bar_canvas_);
      canvas->alignXCenter(0);
      canvas->alignYBottomOutside(0);
      canvas->relativeDepth(-0.01f);
      canvas->absoluteWidth(220);
      canvas->absoluteHeight(30);
    }

    { // put the label above the slider
      alignment_label_.textLabel()->setAlignment(GUIALIGN_CENTER);
      alignment_label_.textLabel()->setText("Alignment");
      GUICanvas* canvas = alignment_label_.getCanvas();
      canvas->setSourceCanvas(&alignment_background_canvas_);
      canvas->alignXCenter(0);
      canvas->alignYTopOutside(-0);
      canvas->relativeDepth(-0.01f);
      canvas->absoluteWidth(220);
      canvas->absoluteHeight(30);
    }
    {
      race_class_text_.textLabel()->setAlignment(GUIALIGN_LEFT_TOP);
      GUICanvas* canvas = race_class_text_.getCanvas();
      canvas->setSourceCanvas(myAvatarPanel.getCanvas());
      canvas->alignXLeft(+20);
      canvas->alignYTop(+15);
      canvas->relativeDepth(-0.01f);
      canvas->absoluteWidth(220);
      canvas->absoluteHeight(0);
    }
    {
      gold_label_.textLabel()->setAlignment(GUIALIGN_LEFT_TOP);
      gold_label_.textLabel()->setTextColor(D3DCOLOR_XRGB(200,200,0));
      GUICanvas* canvas = gold_label_.getCanvas();
      canvas->setSourceCanvas(myAvatarPanel.getCanvas());
      canvas->alignXLeft(+20);
      canvas->alignYBottom(-15);
      canvas->relativeDepth(-0.01f);
      canvas->absoluteWidth(220);
      canvas->absoluteHeight(25);
    }
}

