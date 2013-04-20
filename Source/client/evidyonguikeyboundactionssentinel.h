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
#ifndef __EVIDYONGUIKEYBOUNDACTIONSSENTINEL_H__
#define __EVIDYONGUIKEYBOUNDACTIONSSENTINEL_H__
#pragma once


#include "common/gui/guikeyboard.h"
#include "actiontargeter.h"
#include "shared/item/itemindex.h"
#include "shared/spell/spellindex.h"

namespace dc {
class dcStreamOut;
class dcStreamIn;
}

namespace Evidyon {

class ActionTargeter;
struct ClientAvatarInventoryItem;

namespace Item {
struct ItemClientDescription;
}
namespace Spell {
struct SpellClientDescription;
}
}

namespace Evidyon {

enum ActionType;

struct KeyBoundAction {
  enum Type {
    INVALID,
    COMMAND,
    CAST_SPELL,
    CONSUME_ITEM,
  } type;
  union {
    ActionType action;
    Spell::SpellIndex spell_index;
    Item::ItemIndex item_index;
  };
};

  
//----[  EvidyonGUIKeyBoundActionsSentinel  ]----------------------------------
class EvidyonGUIKeyBoundActionsSentinel : public GUIKeyboardSentinel {
public:
  EvidyonGUIKeyBoundActionsSentinel(ActionTargeter* targeter);

  void create(const ClientAvatarInventoryItem* inventory,
              const Item::ItemClientDescription* item_descriptions,
              const Spell::SpellClientDescription* spells);

  void setKey(GUIKey key, const KeyBoundAction* action);

  void setDefaultKeyBindings();
  void saveKeyBindings(dc::dcStreamOut* stream);
  bool readKeyBindings(dc::dcStreamIn* stream); // returns 'false' if the default keys were used

  // Finds the key that activates the given spell
  GUIKey getKeyForSpell(Spell::SpellIndex spell_type);
  GUIKey getKeyForCommand(ActionType command_action);
  GUIKey getKeyForConsumable(Item::ItemIndex item_type);

  // Finds the key that produces the given action
  GUIKey getKeyForAction(const KeyBoundAction* action);

  virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  virtual bool onKeyUp(GUIKey key, GUIKeyboardInputSource* keyboard);

private:
  ActionTargeter* targeter_;
  Action actions_[GUIKEY_COUNT];
  KeyBoundAction bound_actions_[GUIKEY_COUNT];
  const ClientAvatarInventoryItem* inventory_;
  const Item::ItemClientDescription* item_descriptions_;
  const Spell::SpellClientDescription* spells_;
};


}



#endif

