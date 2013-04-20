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
#include "evidyonguikeyboundactionssentinel.h"
#include "actiontargeter.h"
#include "shared/spell/spellclientdescription.h"
#include "shared/avatar/evidyon_avatarinventory.h"
#include "shared/item/itemclientdescription.h"

#include "client/actionqueue.h"

#include "shared/evidyon_version.h"

#include <dc/stream>

namespace Evidyon {


static const char KEY_BINDING_VERSION_TEXT[] = "3.0.0"; // change this to invalidate key bindings




//----[  EvidyonGUIKeyBoundActionsSentinel  ]----------------------------------
EvidyonGUIKeyBoundActionsSentinel::EvidyonGUIKeyBoundActionsSentinel(ActionTargeter* targeter) {
  targeter_ = targeter;
  inventory_ = NULL;
  item_descriptions_ = NULL;
  spells_ = NULL;
}




//----[  create  ]-------------------------------------------------------------
void EvidyonGUIKeyBoundActionsSentinel::create(const ClientAvatarInventoryItem* inventory,
                                               const Item::ItemClientDescription* item_descriptions,
                                               const Spell::SpellClientDescription* spells) {
  inventory_ = inventory;
  item_descriptions_ = item_descriptions;
  spells_ = spells;
  setDefaultKeyBindings();
}



//----[  setKey  ]-------------------------------------------------------------
void EvidyonGUIKeyBoundActionsSentinel::setKey(GUIKey key, const KeyBoundAction* action) {
  if (action == NULL) {
    bound_actions_[key].type = KeyBoundAction::INVALID;
  } else {
    bound_actions_[key] = *action;
  }
}



//----[  setDefaultKeyBindings  ]----------------------------------------------
void EvidyonGUIKeyBoundActionsSentinel::setDefaultKeyBindings() {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    bound_actions_[i].type = KeyBoundAction::INVALID;
  }

  bound_actions_[GUIKEY_I].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_I].action = ACTION_OPEN_INVENTORY;

  bound_actions_[GUIKEY_U].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_U].action = ACTION_OPEN_USEITEM;

  bound_actions_[GUIKEY_C].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_C].action = ACTION_OPEN_CAST;

  bound_actions_[GUIKEY_D].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_D].action = ACTION_OPEN_DROP;
  
  bound_actions_[GUIKEY_S].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_S].action = ACTION_OPEN_STAT_DISPLAY;

  bound_actions_[GUIKEY_RETURN].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_RETURN].action = ACTION_OPEN_CHAT;

  bound_actions_[GUIKEY_F5].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_F5].action = ACTION_TAKE_SCREENSHOT;

  bound_actions_[GUIKEY_L].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_L].action = ACTION_TOGGLE_CHAT_LOG;

  bound_actions_[GUIKEY_GRAVE].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_GRAVE].action = ACTION_TOGGLE_NAME_DISPLAY;

  bound_actions_[GUIKEY_TAB].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_TAB].action = ACTION_STOP_ACTION;

  bound_actions_[GUIKEY_F1].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_F1].action = ACTION_SHOW_HELP;

  bound_actions_[GUIKEY_V].type = KeyBoundAction::COMMAND;
  bound_actions_[GUIKEY_V].action = ACTION_TOGGLE_VIEW_ANGLE;
}



//----[  saveKeyBindings  ]----------------------------------------------------
void EvidyonGUIKeyBoundActionsSentinel::saveKeyBindings(dc::dcStreamOut* stream) {
  stream->writeString(std::string(KEY_BINDING_VERSION_TEXT));
  stream->write(bound_actions_, sizeof(bound_actions_));
}



//----[  readKeyBindings  ]----------------------------------------------------
bool EvidyonGUIKeyBoundActionsSentinel::readKeyBindings(dc::dcStreamIn* stream) {
  std::string str;
  if (!stream->readString(&str) || 0 != str.compare(KEY_BINDING_VERSION_TEXT)) {
    setDefaultKeyBindings();
    return false;
  } else {
    stream->read(bound_actions_, sizeof(bound_actions_));

    return true;
  }
}



//----[  getKeyForSpell  ]-----------------------------------------------------
GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForSpell(Spell::SpellIndex spell_type) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (bound_actions_[i].type == KeyBoundAction::CAST_SPELL &&
        bound_actions_[i].spell_index == spell_type) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}



//----[  getKeyForUserEvent  ]-------------------------------------------------
GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForCommand(ActionType command_action) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (bound_actions_[i].type == KeyBoundAction::COMMAND &&
        bound_actions_[i].action == command_action) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}




//----[  getKeyForConsumable  ]------------------------------------------------
GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForConsumable(Item::ItemIndex item_type) {
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (bound_actions_[i].type == Action::TYPE_CONSUMABLE &&
        bound_actions_[i].item_index == item_type) return (GUIKey)i;
  }
  return GUIKEY_NULL;
}




//----[  getKeyForAction  ]----------------------------------------------------
GUIKey EvidyonGUIKeyBoundActionsSentinel::getKeyForAction(const KeyBoundAction* action) {
  KeyBoundAction::Type type = action->type;
  for (int i = 0; i < GUIKEY_COUNT; ++i) {
    if (bound_actions_[i].type != type) continue;
    switch (type) {
    case KeyBoundAction::CAST_SPELL: if (bound_actions_[i].spell_index == action->spell_index) return (GUIKey)i; break;
    case KeyBoundAction::CONSUME_ITEM: if (bound_actions_[i].item_index == action->item_index) return (GUIKey)i; break;
    case KeyBoundAction::COMMAND: if (bound_actions_[i].action == action->action) return (GUIKey)i; break;
    }
  }
  return GUIKEY_NULL;
}



//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIKeyBoundActionsSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) { // prevent propagation
    return true;
  }

  if (key >= GUIKEY_1 && key <= GUIKEY_0) { // keyboard-order
    if (targeter_->onNumberKey((key == GUIKEY_0) ? 0 : (key - GUIKEY_1 + 1))) {
      // pressed a number key, and the request was processed
      return true;
    }
  }

  switch (bound_actions_[key].type) {
    case Action::TYPE_USEREVENT:
      Evidyon::Client::ActionQueue::singleton()->addCommand(bound_actions_[key].action);
      break;
    case Action::TYPE_SPELL: {
      Spell::SpellIndex type = bound_actions_[key].spell_index;
      targeter_->castSpell(type, &spells_[type]);
    } break;
    case Action::TYPE_CONSUMABLE: {
      int type = bound_actions_[key].item_index;
      const Item::ItemClientDescription* description = &item_descriptions_[type];
      if (false == description->consumable) break;
      // this searches items from the end of the inventory to the start,
      // so that smaller stacks--which are usually last--are consumed first.
      for (int i = Evidyon::AVATAR_INVENTORY_SIZE - 1; i >= 0; --i) {
        if (inventory_[i].type == type) {
          targeter_->consumeItem(i, description->consuming_requires_target, description->consuming_requires_slot);
          break;
        }
      }
    } break;

    default: return false;
  }

  // capture this event
  return true;
}



//----[  onKeyUp  ]------------------------------------------------------------
bool EvidyonGUIKeyBoundActionsSentinel::onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (key == GUIKEY_ESCAPE) {
    targeter_->userEvent(EVENT_ESCAPE);
    return true;
  }

  return false;
}



}

