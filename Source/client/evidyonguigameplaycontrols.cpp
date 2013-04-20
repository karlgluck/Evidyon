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
#include "evidyonguigameplaycontrols.h"
#include "common/gui/guilayer.h"
#include <dc/debug>
#include <dc/filestream>
#include "shared/item/itemclientdescription.h"
#include "shared/spell/spellclientdescription.h"
#include "client/actionqueue.h"


namespace Evidyon {



  
//----[  MouseSentinel  ]------------------------------------------------------
EvidyonGUIGameplayControls::MouseSentinel::MouseSentinel(
    EvidyonGUIGameplayControls* owner) {
  owner_ = owner;
}




//----[  onMouseButtonAction  ]------------------------------------------------
bool EvidyonGUIGameplayControls::MouseSentinel::onMouseButtonAction(
    GUIMouseButtonAction action,
    GUIPoint position,
    GUIMouseInputSource* mouse) {
  if (!owner_->isActive()) return false;
  if (owner_->isBinding()) {
    if ((action == GUIMOUSE_DOWN || action == GUIMOUSE_ALT_DOWN)) {
      owner_->cancelBinding();
    }
  } else {
    bool return_value = GUIMouseSentinel::onMouseButtonAction(action, position, mouse);
    if (action == GUIMOUSE_CLICK_ALT_SINGLE) {
      obtainMouseFocus();
      mouse->click();
      started_binding_position_ = position;
      owner_->startBinding();
      if (!owner_->isBinding()) { // if nothing is being bound, move the mouse so that highlighting works again
        mouse->generateMovementUpdate();
      }
    } else {
      return return_value;
    }
  }

  // eat this message
  return true;
}




//----[  onMouseMove  ]--------------------------------------------------------
bool EvidyonGUIGameplayControls::MouseSentinel::onMouseMove(GUIPoint position,
                                                            GUIPoint oldPosition,
                                                            GUIMouseInputSource* mouse) {
  if (owner_->isBinding()) {
    int x = position.x - started_binding_position_.x;
    int y = position.y - started_binding_position_.y;
    if (abs(x) > 10 || abs(y) > 10) owner_->cancelBinding();
  }

  return GUIMouseSentinel::onMouseMove(position, oldPosition, mouse);
}





//----[  KeyboardSentinel  ]---------------------------------------------------
EvidyonGUIGameplayControls::KeyboardSentinel::KeyboardSentinel(EvidyonGUIGameplayControls* owner) {
  owner_ = owner;
}






//----[  onKeyDown  ]----------------------------------------------------------
bool EvidyonGUIGameplayControls::KeyboardSentinel::onKeyDown(GUIKey key, GUIKeyboardInputSource* keyboard) {
  if (owner_->isBinding()) {
    owner_->bind(key);
    return true;
  } else {
    return GUIKeyboardSentinel::onKeyDown(key, keyboard);
  }
}





//----[  startBinding  ]-------------------------------------------------------
void EvidyonGUIGameplayControls::startBinding() {
  tooltip_box_.becomeOwner(&mouse_sentinel_);
  tooltip_box_.addContent(&binding_description_);
  tooltip_box_.showContentAfter(0.0);

  keyboard_sentinel_.obtainKeyboardFocus();


  const Spell::SpellClientDescription* spell;
  int index;

  if (spells_.getClickedSpell(&index, &spell)) {
    std::string text = "Press key to bind ";
    text.append(spell->name);
    text.append("\n[del] - erase");
    GUISize panel_size;
    binding_description_.set(text, &panel_size);
    tooltip_box_.setContentSize(panel_size);
    current_binding_.type = KeyBoundAction::CAST_SPELL;
    current_binding_.spell_index = index;
    return;
  }

  // bind a consumable OR split a stack
  const ClientAvatarInventoryItem* inventory_item;
  const Item::ItemClientDescription* item_description;
  if (unequipped_.getClickedItem(&inventory_item, &item_description)) {
    if (current_dialog_ == DIALOG_USE_ITEM) {
      std::string text = "Press key to bind to ";
      text.append(item_description->name);
      text.append("\n[del] - erase");
      GUISize panel_size;
      binding_description_.set(text, &panel_size);
      tooltip_box_.setContentSize(panel_size);
      current_binding_.type = KeyBoundAction::CONSUME_ITEM;
      current_binding_.item_index = inventory_item->type;
      return;
    } else {
      // split a stack at the clicked item
      // This code was added 5/12/2009, long after the original was written because
      // it's the best place to make right-click split an item stack.
      targeter_.splitStackedItem()->inventory_index_ = inventory_item->inventory_index;
    }
  }

  // clear out the other lists to make sure the user doesn't accidentally
  // perform an action they were not intending.
  equipped_.getClickedItem(NULL, NULL);
  storage_.getClickedItem(NULL, NULL);

  // try finding a basic event type
  UserEventType user_event_type = INVALID_ACTION;
  const char* desc = NULL;
  EvidyonGUIGameDialogMenu::Button button = dialog_menu_.pollSelectionButtons();
  if (button < EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS) {
    std::string text = "Press key to bind '";
    text.append(EvidyonGUIGameDialogMenu::getButtonText(button));
    text.append("'\n[del] - erase");
    GUISize panel_size;
    binding_description_.set(text, &panel_size);
    tooltip_box_.setContentSize(panel_size);
    current_binding_.action = EvidyonGUIGameDialogMenu::getButtonActionType(button);
    current_binding_.type = KeyBoundAction::COMMAND;
    return;
  }

  cancelBinding();
}







//----[  cancelBinding  ]------------------------------------------------------
void EvidyonGUIGameplayControls::cancelBinding() {
  keyboard_sentinel_.releaseKeyboardFocus();
  tooltip_box_.releaseOwnership(&mouse_sentinel_);
}






//----[  isBinding  ]----------------------------------------------------------
bool EvidyonGUIGameplayControls::isBinding() {
  return tooltip_box_.hasOwnership(&mouse_sentinel_);
}



//----[  isActive  ]-----------------------------------------------------------
bool EvidyonGUIGameplayControls::isActive() {
  return current_dialog_ != DIALOG_NONE;
}



//----[  bind  ]---------------------------------------------------------------
void EvidyonGUIGameplayControls::bind(GUIKey key) {
  if (key == GUIKEY_ESCAPE) {
    cancelBinding();
    return;
  }

  key_bound_actions_.setKey(
    key_bound_actions_.getKeyForAction(
      &current_binding_), NULL);

  if (key != GUIKEY_DELETE) {
    key_bound_actions_.setKey(key, &current_binding_);
  }


  // if the user tries to unequip all of the dialog menu keys, re-equip
  // at least 1 key
  {
    int i = 0;
    for (; i < EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS; ++i) {
      if (GUIKEY_NULL != key_bound_actions_.getKeyForCommand(
                            EvidyonGUIGameDialogMenu::getButtonActionType(
                              (EvidyonGUIGameDialogMenu::Button)i))) {
        break;
      }
    }
    if (i == EvidyonGUIGameDialogMenu::NUMBER_OF_MENU_BUTTONS) {
      Evidyon::KeyBoundAction action;
      action.type = KeyBoundAction::COMMAND;
      action.action = ACTION_OPEN_INVENTORY;
      key_bound_actions_.setKey(GUIKEY_I, &action);
    }
  }

  dialog_menu_.sync(&key_bound_actions_);
  spells_.sync(&key_bound_actions_, spell_availability_mask_);

  if (current_dialog_ == DIALOG_USE_ITEM) { // update consumable key bindings
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
  }

  cancelBinding();
}




//----[  saveKeyBindings  ]----------------------------------------------------
void EvidyonGUIGameplayControls::saveKeyBindings() {
  dc::dcFileStream fs;
  fs.open(keys_file_.c_str(), STREAM_OVERWRITE);
  key_bound_actions_.saveKeyBindings(&fs);
  fs.close();
}




//----[  EvidyonGUIGameplayControls  ]-----------------------------------------
EvidyonGUIGameplayControls::EvidyonGUIGameplayControls()
    : keyboard_sentinel_(this),
      mouse_sentinel_(this),
      key_bound_actions_(&targeter_) {
  primary_layer_ = NULL;
  spell_availability_mask_ = NULL;

  key_bound_actions_.setParentKeyboardSentinel(&keyboard_sentinel_);
  last_update_cursor_targeting_ = false;
}




//----[  isReadingSlot  ]------------------------------------------------------
bool EvidyonGUIGameplayControls::isReadingSlot() const {
  return targeter_.isReadingSlot();
}


//----[  create  ]-------------------------------------------------------------
void EvidyonGUIGameplayControls::create(const std::string& keys_file,
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
                                        size_t number_of_quests) {
  keys_file_ = keys_file;
  primary_layer_ = primary_layer;
  keyboard_sentinel_.setParentKeyboardSentinel(root_keyboard_sentinel);
  mouse_sentinel_.setParentMouseSentinel(root_mouse_sentinel);
  dialog_menu_.setFont(font);
  dialog_menu_.setParentSentinels(&mouse_sentinel_, NULL);
  spells_.create(primary_layer->getPrimaryCanvas(), font, spells, number_of_spells, &tooltip_box_);
  spells_.setParentSentinels(&mouse_sentinel_, NULL);
  equipped_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  equipped_.setParentSentinels(&mouse_sentinel_, NULL);
  unequipped_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  unequipped_.setParentSentinels(&mouse_sentinel_, NULL);
  tooltip_box_.create(primary_layer, 20, mouse_cursor_canvas);
  spell_availability_mask_ = spell_availability_mask;


  chat_text_line_.create(font, &mouse_sentinel_, &keyboard_sentinel_, hpMpXpSize);

  stat_display_.create(font, font, primary_layer, &mouse_sentinel_, &keyboard_sentinel_, NULL);

  targeter_.create(camera, actor_manager);

  targeter_.setParentMouseSentinel(root_mouse_sentinel);
  targeter_.disable();

  storage_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, storage, &tooltip_box_);
  storage_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  trade_dialog_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  trade_dialog_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  bazaar_.create(primary_layer->getPrimaryCanvas(), font, item_descriptions, number_of_item_descriptions, inventory, &tooltip_box_);
  bazaar_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  quantity_prompt_.create(primary_layer->getPrimaryCanvas(), font);
  quantity_prompt_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  quest_proposal_.create(primary_layer->getPrimaryCanvas(), font, quests, number_of_quests);
  quest_proposal_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  help_dialog_.create(primary_layer->getPrimaryCanvas(), font);
  help_dialog_.setParentSentinels(&mouse_sentinel_, &keyboard_sentinel_);

  CONFIRM(active_quest_display_.create(font,
                                       primary_layer->getPrimaryCanvas(),
                                       &targeter_));
  active_quest_display_.setParentSentinels(&mouse_sentinel_, NULL);

  {
    GUICanvas* canvas = dialog_menu_.getCanvas();
    canvas->setSourceCanvas(primary_layer->getPrimaryCanvas());
    canvas->alignXLeft(0);
    canvas->alignYTop(0);
    canvas->absoluteDepth(0.2f);
    canvas->relativeHeight(0);
    canvas->relativeWidth(0);
  }
  {
    drop_gold_.setFont(font);
    drop_gold_.setText("Drop Gold");
    drop_gold_.disable();
    drop_gold_.setParentSentinels(&mouse_sentinel_, NULL);
    GUICanvas* canvas = drop_gold_.getCanvas();
    canvas->setSourceCanvas(unequipped_.getCanvas());
    canvas->alignXCenter(0);
    canvas->alignYBottomOutside(5);
    canvas->relativeDepth(0.0);
    canvas->relativeWidth(-30);
    canvas->absoluteHeight(35);
  }

  binding_description_.setFont(font);
  key_bound_actions_.create(inventory, item_descriptions, spells);

  {
    current_dialog_ = DIALOG_INVENTORY; // make sure hideDialogs does something
    hideDialogs();
  }

  {
    bool keys_reset = true;
    dc::dcFileStream fs;
    if (fs.open(keys_file.c_str(), 0)) {
      keys_reset = false == key_bound_actions_.readKeyBindings(&fs);
    }
    fs.close();
    //if (keys_reset) { // first-login, display help screen
    //  showDialog(EvidyonGUIGameplayControls::DIALOG_HELP);
    //}
  }
}




//----[  destroy  ]------------------------------------------------------------
void EvidyonGUIGameplayControls::destroy() {
  saveKeyBindings();
  if (primary_layer_) {
    dialog_menu_.unregisterElements(primary_layer_);
    drop_gold_.unregisterElements(primary_layer_);
  }
  help_dialog_.destroy();
  bazaar_.destroy();
  spells_.destroy();
  equipped_.destroy();
  unequipped_.destroy();
  tooltip_box_.destroy();
  chat_text_line_.destroy();
  stat_display_.destroy();
  storage_.destroy();
  trade_dialog_.destroy();
  quantity_prompt_.destroy();
  quest_proposal_.destroy();
  active_quest_display_.destroy();
}






//----[  getCursorState  ]-----------------------------------------------------
int EvidyonGUIGameplayControls::getCursorState() const {
  return last_update_cursor_targeting_ ? 1 : 0;
}




//----[  update  ]-------------------------------------------------------------
Action::Type EvidyonGUIGameplayControls::update(double time,
                                                const ActionInstance** action_instance,
                                                bool* change_cursor_state) {

  if (current_dialog_ != DIALOG_NONE) {

    if (isBinding()) {
      tooltip_box_.update(time);
      return Action::TYPE_INVALID;
    }

    dialog_menu_.updateSelectionButtons();
  }

  if (current_dialog_ != DIALOG_NONE) {

    if (current_dialog_ == DIALOG_CHAT) chat_text_line_.update();
    if (current_dialog_ == DIALOG_STAT_DISPLAY) {
      int ability;
      if (stat_display_.pollAddAbilityPoint(&ability)) {
        targeter_.attributesAddAbilityPoint()->ability = ability;
      }
    }

    const ClientAvatarInventoryItem* inventory_item;
    const Item::ItemClientDescription* item_description;

    if (equipped_.getClickedItem(&inventory_item, &item_description)) {
      switch (current_dialog_) {
      case DIALOG_INVENTORY:
        targeter_.unequipItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      }
    }

    if (unequipped_.getClickedItem(&inventory_item, &item_description)) {
      switch (current_dialog_) {
      case DIALOG_INVENTORY: // equip
        targeter_.equipItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_DROP_ITEMS: // drop
        targeter_.dropItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_DROP_SACRIFICE_ITEMS:
        // the geosid is filled in by the processor
        Client::ActionQueue::add<Action_Geosid_SacrificeItem>()
          ->inventory_index = inventory_item->inventory_index;
        break;
      case DIALOG_USE_ITEM: // use
        CONFIRM(item_description->consumable) else break;
        targeter_.consumeItem(inventory_item->inventory_index,
                              item_description->consuming_requires_target,
                              item_description->consuming_requires_slot);
        hideDialogs();
        break;
      case DIALOG_STORAGE:
        targeter_.storageDepositItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      case DIALOG_GEOSID:
        targeter_.geosidSacrificeItem()->inventory_index_ = inventory_item->inventory_index;
        break;
      }
    }

    if (current_dialog_ == DIALOG_DROP_ITEMS) {
      if (quantity_prompt_.isActive()) {
        if (quantity_prompt_.pollOK()) {
          targeter_.dropCurrency()->amount_ = quantity_prompt_.getValue();
          quantity_prompt_.hide();
          hideDialogs();
        }
        if (quantity_prompt_.pollCancel()) {
          quantity_prompt_.hide();
        }
      } else {
        if (drop_gold_.pollPushed()) {
          quantity_prompt_.show(primary_layer_);
          quantity_prompt_.initialize("How much gold should be dropped?", "Drop", "Cancel");
        }
      }
    }

    if (current_dialog_ == DIALOG_STORAGE) {
      const ItemInStorage* storage_item;
      if (storage_.getClickedItem(&storage_item, &item_description)) {
        targeter_.storageWithdrawItem()->inventory_index_ = storage_item->index;
      }
    }

    // Poll the trade dialog for the events it generates
    if (current_dialog_ == DIALOG_TRADE) {
      trade_dialog_.update(&targeter_);
    }

    // Poll the quest dialog
    if (current_dialog_ == DIALOG_QUEST_PROPOSAL) {
      quest_proposal_.updateQuestProposal(&targeter_);
    }

    if (current_dialog_ == DIALOG_ACTIVE_QUESTS) {
      unsigned int quest_index;
      int client_quest_index;
      if (active_quest_display_.update(time,
                                       &client_quest_index,
                                       &quest_index)) {
        quest_proposal_.displayActiveQuest(
          quest_index,
          client_quest_index,
          time);
        quest_proposal_.show(active_quest_display_.hide());
      }
      quest_proposal_.updateActiveQuest(&targeter_, time);
    }

    tooltip_box_.update(time);
    unequipped_.updateTooltip();
    equipped_.updateTooltip();
    switch (current_dialog_) {
      case DIALOG_CAST_SPELL: {
        spells_.updateTooltip();
        const Spell::SpellClientDescription* spell_description;
        int index;
        if (spells_.getClickedSpell(&index, &spell_description)) {
          targeter_.castSpell(index, spell_description);
          hideDialogs();
        }
      } break;
      case DIALOG_STORAGE: storage_.updateTooltip(); break;
    }

    if (current_dialog_ == DIALOG_BAZAAR) {
      bazaar_.update(&targeter_);
    }
  }


  Action::Type action_type = targeter_.update(time, action_instance);
  if (action_type == Action::TYPE_MOVE) { // inspect!
    hideDialogs();
  }

  if (last_update_cursor_targeting_ != targeter_.isTargeting()) {
    last_update_cursor_targeting_ = !last_update_cursor_targeting_;
    *change_cursor_state = true;
  } else {
    *change_cursor_state = false;
  }

  return action_type;
}




//----[  syncInventory  ]------------------------------------------------------
void EvidyonGUIGameplayControls::syncInventory() {
  equipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_EQUIPPED, NULL);
  trade_dialog_.syncInventory();
  bazaar_.syncInventory();
  if (current_dialog_ == DIALOG_USE_ITEM) {
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
  } else {
    unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);

    // todo: this is duplicated from showDialog to fix display inconsistency
    switch (current_dialog_) {
      case DIALOG_GEOSID:
      unequipped_.setText("Select Items to Sacrifice for Geonite");
      break;
      case DIALOG_DROP_ITEMS:
      unequipped_.setText("Drop Items");
      break;
      case DIALOG_DROP_SACRIFICE_ITEMS:
      unequipped_.setText("Sacrifice Items to Geosid for Geonite");
      break;
    }
  }
}




//----[  syncSpells  ]---------------------------------------------------------
void EvidyonGUIGameplayControls::syncSpells() {
  spells_.sync(&key_bound_actions_, spell_availability_mask_);
}




//----[  syncStorageItems  ]---------------------------------------------------
void EvidyonGUIGameplayControls::syncStorageItems() {
  storage_.sync();
}




//----[  syncTrade  ]----------------------------------------------------------
void EvidyonGUIGameplayControls::syncTrade(
    const char* their_name,
    CurrencyType my_offered_currency,
    unsigned char* my_items, unsigned int number_of_my_items,
    CurrencyType their_offered_currency,
    TradeItemDescription* their_items, unsigned int number_of_their_items) {
  showDialog(DIALOG_TRADE);
  trade_dialog_.syncTrade(their_name,
                          my_offered_currency,
                          my_items, number_of_my_items,
                          their_offered_currency,
                          their_items, number_of_their_items);
}



//----[  syncBazaar  ]---------------------------------------------------------
void EvidyonGUIGameplayControls::syncBazaar(
    BazaarCategory category,
    ItemInBazaar* items_in_category,
    size_t number_of_items) {
  showDialog(DIALOG_BAZAAR);
  bazaar_.sync(category, items_in_category, number_of_items);
}



//----[  promptQuestProposal  ]------------------------------------------------
void EvidyonGUIGameplayControls::promptQuestProposal(
    unsigned int quest_index,
    Evidyon::Actor::ActorID npc_actor_id,
    int npc_quest_index,
    unsigned int validation_number) {
  quest_proposal_.displayQuestProposal(quest_index,
                                       npc_actor_id,
                                       npc_quest_index,
                                       validation_number);
  showDialog(DIALOG_QUEST_PROPOSAL);
}



//----[  syncActiveQuestDisplay  ]---------------------------------------------
void EvidyonGUIGameplayControls::syncActiveQuestDisplay(
    const Evidyon::ActiveQuestData quest_data[3]) {
  active_quest_display_.sync(quest_data);
}

//----[  toggleDialog  ]-------------------------------------------------------
void EvidyonGUIGameplayControls::toggleDialog(Dialog dialog) {
  if (current_dialog_ == dialog) {
    hideDialogs();
  } else {
    showDialog(dialog);
  }
}




//----[  showDialog  ]---------------------------------------------------------
void EvidyonGUIGameplayControls::showDialog(Dialog dialog) {
  if (current_dialog_ == dialog) return;

  hideDialogs();

  // TODO: reset the scroll bar positions of the lists here

  // make sure movement messages don't get passed
  targeter_.disable();

  switch (dialog) {
    case DIALOG_INVENTORY:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSplitInventoryCanvases();
      equipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_EQUIPPED, NULL);
      equipped_.show(primary_layer_);
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.validateScrollBar();
      unequipped_.show(primary_layer_);
      break;
    case DIALOG_USE_ITEM:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_CONSUMABLES, &key_bound_actions_);
      unequipped_.show(primary_layer_);
      break;
    case DIALOG_DROP_ITEMS:
      unequipped_.setBackgroundColor(0xA0FFFFFF);
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.validateScrollBar();
      unequipped_.show(primary_layer_);
      unequipped_.setText("Drop Items");
      drop_gold_.registerElements(primary_layer_);
      drop_gold_.enable();
      break;
    case DIALOG_DROP_SACRIFICE_ITEMS:
      unequipped_.setBackgroundColor(0xA0FF0000);
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.validateScrollBar();
      unequipped_.show(primary_layer_);
      unequipped_.setText("Sacrifice Items to Geosid for Geonite");
      break;
    case DIALOG_CAST_SPELL:
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      setSpellCanvas();
      spells_.sync(&key_bound_actions_, spell_availability_mask_);
      spells_.show(primary_layer_);
      break;
    case DIALOG_STAT_DISPLAY:
      targeter_.attributesAddAbilityPoint()->ability = -1; // ask the server for a stat-screen sync
      dialog_menu_.sync(&key_bound_actions_);
      dialog_menu_.registerElements(primary_layer_);
      stat_display_.show();
      break;
    case DIALOG_CHAT:
      chat_text_line_.show(primary_layer_);
      targeter_.enable();
      break;
    case DIALOG_STORAGE:
      setStorageCanvases();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.show(primary_layer_);
      storage_.resetStorageContent(); // clears the storage, pending server message
      storage_.show(primary_layer_);
      break;
    case DIALOG_GEOSID:
      setSingleInventoryCanvas();
      unequipped_.sync(EvidyonGUIModule_InventoryList::DISPLAY_UNEQUIPPED, NULL);
      unequipped_.validateScrollBar();
      unequipped_.show(primary_layer_);
      unequipped_.setText("Select Items to Sacrifice for Geonite");
      break;
    case DIALOG_TRADE:
      trade_dialog_.beginTrade();
      trade_dialog_.syncInventory();
      trade_dialog_.show(primary_layer_);
      break;
    case DIALOG_BAZAAR:
      //targeter_.bazaarGetListings()->category_ = CATEGORY_OWNITEMS;
      Evidyon::Client::ActionQueue::add<Action_BazaarGetListings>()
        ->category = CATEGORY_OWNITEMS;
      bazaar_.changeCategory(CATEGORY_OWNITEMS);
      bazaar_.show(primary_layer_);
      break;
    case DIALOG_QUEST_PROPOSAL:
      quest_proposal_.show(primary_layer_);
      break;
    case DIALOG_ACTIVE_QUESTS:
      active_quest_display_.show(primary_layer_);
      break;
    case DIALOG_HELP:
      help_dialog_.show(primary_layer_);
      break;
  }

  current_dialog_ = dialog;
}





//----[  hideDialog  ]---------------------------------------------------------
void EvidyonGUIGameplayControls::hideDialog(Dialog dialog) {
  if (current_dialog_ == dialog) {
    hideDialogs();
  }
}




//----[  hideDialogs  ]--------------------------------------------------------
bool EvidyonGUIGameplayControls::hideDialogs() {
  if (current_dialog_ == DIALOG_NONE) return false;
  if (current_dialog_ == DIALOG_TRADE) { // switching out of trade? cancel it.
    targeter_.userEvent(TRADE_CANCEL);
  }
  help_dialog_.hide();
  targeter_.enable();
  current_dialog_ = DIALOG_NONE;
  bazaar_.hide();
  equipped_.hide();
  unequipped_.hide();
  spells_.hide();
  chat_text_line_.show(NULL);
  dialog_menu_.unregisterElements(primary_layer_);
  tooltip_box_.becomeOwner(this);
  stat_display_.hideDisplay();
  storage_.hide();
  quantity_prompt_.hide();
  drop_gold_.unregisterElements(primary_layer_);
  drop_gold_.disable();
  trade_dialog_.endTrade(); // .hide() ?
  quest_proposal_.hide();
  active_quest_display_.hide();
  return true;
}




//----[  setSplitInventoryCanvases  ]------------------------------------------
void EvidyonGUIGameplayControls::setSplitInventoryCanvases() {
  {
    GUICanvas* canvas = equipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYTop(20+35+20);
    canvas->absoluteHeight(220);
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.4f);
    equipped_.scrollToTop();
  }
  {
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYBottom(-20);
    canvas->relativeHeight(-(40+35+20+240));
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.4f);
    unequipped_.scrollToTop();
  }
}




//----[  setSingleInventoryCanvas  ]-------------------------------------------
void EvidyonGUIGameplayControls::setSingleInventoryCanvas() {
  {
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(0);
    canvas->alignYBottom(-(20+35));
    canvas->relativeHeight(-(40+35+20+35+5));
    canvas->absoluteWidth(400);
    canvas->absoluteDepth(0.4f);
    unequipped_.scrollToTop();
  }

}



//----[  setSpellCanvas  ]-----------------------------------------------------
void EvidyonGUIGameplayControls::setSpellCanvas() {
  GUICanvas* canvas = spells_.getCanvas();
  canvas->alignXCenter(0);
  canvas->alignYBottom(-20);
  canvas->relativeHeight(-(40+35+20));
  canvas->absoluteWidth(400);
  canvas->absoluteDepth(0.4f);
  spells_.scrollToTop();
}



//----[  setStorageCanvases  ]-------------------------------------------------
void EvidyonGUIGameplayControls::setStorageCanvases() {
  storage_.scrollToTop();
  { // put the storage on the left
    GUICanvas* canvas = storage_.getCanvas();
    canvas->alignXCenter(-350/2-10);
    canvas->alignYCenter(0);
    canvas->relativeHeight(-(40+35+20));
    canvas->absoluteWidth(350);
    canvas->absoluteDepth(0.4f);
    storage_.scrollToTop();
  }
  { // put the inventory on the right
    GUICanvas* canvas = unequipped_.getCanvas();
    canvas->alignXCenter(+350/2+10);
    canvas->alignYCenter(0);
    canvas->relativeHeight(-(40+35+20));
    canvas->absoluteWidth(350);
    canvas->absoluteDepth(0.4f);
    unequipped_.scrollToTop();
  }
}

}

