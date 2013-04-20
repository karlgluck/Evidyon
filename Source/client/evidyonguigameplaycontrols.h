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
#ifndef __EVIDYONGUIGAMEPLAYCONTROLS_H__
#define __EVIDYONGUIGAMEPLAYCONTROLS_H__
#pragma once



#include "evidyonguimodule_inventorylist.h"
#include "evidyonguimodule_spelllist.h"
#include "evidyonguimodule_storagelist.h"
#include "evidyonguimodule_tradedialog.h"
#include "evidyonguigamedialogmenu.h"
#include "evidyonguistatusbars.h"
#include "evidyonguitooltipbox.h"
#include "actiontargeter.h"
#include "evidyonguiactionbindingdescription.h"
//#include "evidyonguimodule_geosiddialog.h"

#include "common/gui/guikeyboard.h"
#include "evidyonguikeyboundactionssentinel.h"

#include "chattextline.h"

#include "evidyonguistatdisplay.h"

#include "evidyonguimodule_bazaar.h"
#include "evidyonguimodule_questproposal.h"
#include "evidyonguimodule_activequestdisplay.h"
#include "evidyonguihelpdialog.h"

namespace Evidyon {
namespace Actor {
namespace Client {
class ClientActorManager;
}
}
}

namespace Evidyon {
namespace Item {
struct ItemClientDescription;
}
}


namespace Evidyon {

class GlobalActorManager;

struct KeyBinding {
  Action action;
};

struct TradeItemDescription;


class EvidyonGUIGameplayControls {
  class KeyboardSentinel : public GUIKeyboardSentinel {
  public:
    KeyboardSentinel(EvidyonGUIGameplayControls* owner);
  public:
    virtual bool onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard);
  private:
    EvidyonGUIGameplayControls* owner_;
  };

  class MouseSentinel : public GUIMouseSentinel {
  public:
    MouseSentinel(EvidyonGUIGameplayControls* owner);

    // This method watches for the key binding shortcut: left + right click
    virtual bool onMouseButtonAction(GUIMouseButtonAction action,
                                     GUIPoint position,
                                     GUIMouseInputSource* mouse);
    // If you move the mouse more than a certain amount, the key binding
    // effect is canceled.
    virtual bool onMouseMove(GUIPoint position,
                             GUIPoint oldPosition,
                             GUIMouseInputSource* mouse);
  private:
    GUIPoint started_binding_position_;
    EvidyonGUIGameplayControls* owner_;
  };

public:
  enum Dialog {
    DIALOG_STAT_DISPLAY,
    DIALOG_INVENTORY,
    DIALOG_DROP_ITEMS,
    DIALOG_DROP_SACRIFICE_ITEMS,
    DIALOG_USE_ITEM,
    DIALOG_CAST_SPELL,
    DIALOG_CHAT,
    DIALOG_KEY_BINDINGS,
    DIALOG_STORAGE,
    DIALOG_GEOSID,
    DIALOG_TRADE,
    DIALOG_BAZAAR,
    DIALOG_QUEST_PROPOSAL,
    DIALOG_ACTIVE_QUESTS,
    DIALOG_HELP,
    DIALOG_NONE,
  };

public:
  EvidyonGUIGameplayControls();
  void create(const std::string& keys_file,
              GUIFont* font,
              GUILayer* primary_layer,
              GUIMouseSentinel* root_mouse_sentinel,
              GUIKeyboardSentinel* root_keyboard_sentinel,
              GUIGenericCanvas* mouse_cursor_canvas,
              const Item::ItemClientDescription* item_descriptions,
              size_t number_of_item_descriptions,
              const ClientAvatarInventoryItem* inventory,
              const Spell::SpellClientDescription* spells,
              size_t number_of_spells,
              Avatar::SpellAvailabilityMask* spell_availability_mask,
              GUISize hpMpXpSize,
              IsometricCamera* camera,
              Actor::Client::ClientActorManager* actor_manager,
              const ItemInStorage* storage,
              const Evidyon::Quests::Client::Quest* quests,
              size_t number_of_quests);

  // Frees all internal references
  void destroy();

  // Polls for user input changes and returns the latest action the
  // user wants to perform.
  // If the change_cursor_state variable is set to 'true', then the
  // cursor icon should be updated based on the return value from getCursorState
  Action::Type update(double time,
                      const ActionInstance** action_instance,
                      bool* change_cursor_state);

  // Returns '1' if the cursor should be in the the targeting state, or
  // '0' if it should be in the normal state.
  int getCursorState() const;

  // Returns 'true' if the action targeter is waiting for the user to select 
  // a numbered slot (1-9).
  bool isReadingSlot() const;

  // Rebuilds the inventory panels' contents.  This should be called whenever
  // the inventory is edited.
  void syncInventory();
  void syncSpells();
  void syncStorageItems();
  void syncTrade(const char* their_name,
                 CurrencyType my_offered_currency,
                 unsigned char* my_items, unsigned int number_of_my_items,
                 CurrencyType their_offered_currency,
                 TradeItemDescription* their_items, unsigned int number_of_their_items);
  void syncBazaar(BazaarCategory category,
                  ItemInBazaar* items_in_category,
                  size_t number_of_items);

  // Displays the quest proposal dialog to the player and updates it with
  // the given information.
  void promptQuestProposal(unsigned int quest_index,
                           Evidyon::Actor::ActorID npc_actor_id,
                           int npc_quest_index,
                           unsigned int validation_number);
  void syncActiveQuestDisplay(const Evidyon::ActiveQuestData quest_data[3]);

//----------------
  // this is a super hackity hacky hack
  inline EvidyonGUIStatDisplay* syncAvatarStats() { return &stat_display_; }
//----------------


  void showDialog(Dialog dialog);

  // Closes the dialog only if it is currently open
  void hideDialog(Dialog dialog);

  // Shows a dialog if it isn't visible, hides all dialogs if it is.
  void toggleDialog(Dialog dialog);

  // This method returns 'false' if no menu was being shown.  This is so that
  // the escape key can be used to close any menu, and when no menu is being
  // displayed it can log the character out of the world.
  bool hideDialogs();


private:
  void setSplitInventoryCanvases();
  void setSingleInventoryCanvas();
  void setSpellCanvas();
  void setStorageCanvases();

private:
  void startBinding();
  void cancelBinding();
  bool isBinding();
  bool isActive();
  void bind(GUIKey key);

private:
  void saveKeyBindings();

private:
  std::string keys_file_;
  GUILayer* primary_layer_;
  EvidyonGUIGameDialogMenu dialog_menu_;
  ActionTargeter targeter_;
  EvidyonGUIModule_SpellList spells_;
  EvidyonGUIModule_InventoryList equipped_, unequipped_;
  EvidyonGUIModule_StorageList storage_;
  EvidyonGUIModule_TradeDialog trade_dialog_;
  EvidyonGUIModule_Bazaar bazaar_;
  EvidyonGUITooltipBox tooltip_box_;
  Avatar::SpellAvailabilityMask* spell_availability_mask_;
  ChatTextLine chat_text_line_;

  EvidyonGUIStatDisplay stat_display_;

  Dialog current_dialog_;

  // Whether or not the cursor was targeting something as of the
  // last call to update()
  bool last_update_cursor_targeting_,
       last_update_reading_slot_; // same, but for a #

  // Displayed in the drop items dialog
  EvidyonGUIButton drop_gold_;

  // Used when dropping items.  (could also be used for splitting stacks?)
  EvidyonGUIModule_QuantityPrompt quantity_prompt_;

  EvidyonGUIModule_QuestProposal quest_proposal_;
  EvidyonGUIModuleActiveQuestDisplay active_quest_display_;

private:
  KeyboardSentinel keyboard_sentinel_;
  MouseSentinel mouse_sentinel_;

  EvidyonGUIActionBindingDescription binding_description_;

  KeyBoundAction current_binding_;

  EvidyonGUIKeyBoundActionsSentinel key_bound_actions_;

  Evidyon::Client::EvidyonGUIHelpDialog help_dialog_;
};

}




#endif