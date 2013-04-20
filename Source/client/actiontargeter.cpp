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
#include "actiontargeter.h"
#include "common/isometriccamera.h"
#include "actor/clientactormanager.h"
#include "shared/actor/actorinteraction.h"
#include "client/actionqueue.h"
#include "client/clientfacade.h"
#include "shared/spell/spellclientdescription.h"

namespace Evidyon {

static const double MOUSE_STICK_TIME = 0.35;
static const float ACTOR_SELECT_RADIUS_SQ = 0.7*0.7;

static const double MOVEMENT_STREAM_PERIOD = 0.1;



//----[  ActionTargeter  ]-----------------------------------------------------
ActionTargeter::ActionTargeter() {
  last_update_time_ = 0.0;
  unstick_mouse_time_ = 0.0;
  mouse_stuck_ = false;
  mouse_world_x_ = 0.0f;
  mouse_world_y_ = 0.0f;
  next_time_to_return_movement_ = 0.0;
  return_movement_ = false;
  camera_ = NULL;
  actor_manager_ = NULL;
  action_is_valid_ = false;
  action_needs_target_ = false;
  action_needs_slot_ = false;
  last_targeted_spell_.spell = NULL;
  last_targeted_spell_.spell_type = Evidyon::Spell::INVALID_SPELL_INDEX;
}




//----[  create  ]-------------------------------------------------------------
void ActionTargeter::create(IsometricCamera* camera,
                            Actor::Client::ClientActorManager* actor_manager) {
  camera_ = camera;
  actor_manager_ = actor_manager;
}




//----[  onMouseMove  ]--------------------------------------------------------
bool ActionTargeter::onMouseMove(GUIPoint position,
                                 GUIPoint oldPosition,
                                 GUIMouseInputSource* mouse) {
  last_mouse_position_ = position;
  return true; // don't propagate movement
}




//----[  onMouseButtonAction  ]------------------------------------------------
bool ActionTargeter::onMouseButtonAction(GUIMouseButtonAction action,
                                           GUIPoint position,
                                           GUIMouseInputSource* mouse) {
  switch (action) {
  case GUIMOUSE_DOWN: {
      return_movement_ = !action_is_valid_;
      next_time_to_return_movement_ = 0.0; // return immediately
      mouse_stuck_ = true;
      unstick_mouse_time_ = last_update_time_ + MOUSE_STICK_TIME;

      // force the mouse to update since the user clicked here
      camera_->getWorldLocation(last_mouse_position_.x,
                                last_mouse_position_.y,
                               &mouse_world_x_,
                               &mouse_world_y_);
    } break;

  case GUIMOUSE_CLICK_SINGLE: {
      setActionTarget(false); // non-geonite-burst
    } break;

  case GUIMOUSE_UP: {
      return_movement_ = false;
      //mouse_stuck_ = false; // don't clear mouse_stuck_ here so that movement clicks work out correctly
    } break;

  case GUIMOUSE_ALT_DOWN: {
    if ((cancelAction() == false) &&
        last_targeted_spell_.spell != NULL) {
      action_instance_.action.type = Action::TYPE_SPELL;
      action_is_valid_ = true;
      action_needs_target_ = true;
      action_needs_slot_ = false;
      action_instance_.action.spell = last_targeted_spell_;
    }
    } break;

  case GUIMOUSE_ALT_UP: {
      if (action_is_valid_) {
        // re-play the last spell cast
        setActionTarget(false);
      }
      return_movement_ = false;
    } break;

  case GUIMOUSE_CLICK_ALT_SINGLE: {
//      setActionTarget(true); // + geonite burst
    } break;
  }

  // block other interaction
  return true;
}




//----[  cancelAction  ]-------------------------------------------------------
bool ActionTargeter::cancelAction() {
  bool was_valid = action_is_valid_;
  action_is_valid_ = false;
  action_instance_.action.type = Action::TYPE_INVALID;
  action_needs_target_ = false;
  action_needs_slot_ = false;
  return was_valid;
}




//----[  setActionTarget  ]----------------------------------------------------
void ActionTargeter::setActionTarget(bool is_geonite_burst) {
  assert(!is_geonite_burst);
  //if (is_geonite_burst) {
  //  if (action_instance_.action.type == Action::TYPE_SPELL &&
  //    action_is_valid_) {
  //  } else {
  //    cancelAction();
  //    return_movement_ = false;

  //    // find a geosid near the target location
  //    Geosid::GeosidIndex geosid;
  //    bool is_white;
  //    if (Client::ClientFacade::singleton()
  //          ->findClickableGeosid(mouse_world_x_,
  //                                mouse_world_y_,
  //                               &geosid,
  //                               &is_white)) {
  //      if (!is_white) {
  //        // burst attack this geosid
  //        Client::ActionQueue::add<Action_Geosid_BurstAttackGeosid>()
  //          ->geosid = geosid;
  //      }
  //    } else {
  //      if (Client::ClientFacade::singleton()
  //            ->isClientInsideGeosid(&geosid, &is_white)) {
  //        Actor::ActorID actor_id;
  //        if (ACTOR_SELECT_RADIUS_SQ <
  //             actor_manager_->getActorClosestToLocation(
  //                false,    // find client
  //                false,    // living only
  //                mouse_world_x_,
  //                mouse_world_y_,
  //                &actor_id)) {
  //          actor_id = Evidyon::Actor::INVALID_ACTOR_ID;
  //        }

  //        // use the geonite burst ability
  //        Action_Geosid_BurstUseGeosid* burst
  //          = Client::ActionQueue::add<Action_Geosid_BurstUseGeosid>();
  //        burst->geosid = geosid;
  //        burst->actor_id = actor_id;
  //        burst->x = mouse_world_x_;
  //        burst->z = mouse_world_y_;
  //      } else {
  //        // burst at the location
  //        Action_Geosid_Burst* burst
  //          = Client::ActionQueue::add<Action_Geosid_Burst>();
  //        burst->x = mouse_world_x_;
  //        burst->z = mouse_world_y_;
  //      }
  //    }

  //    return;
  //  }
  //}

  // don't set if we're waiting on a slot first
  if (action_is_valid_ == false || action_needs_slot_) return;
  action_needs_target_ = false;

  switch (action_instance_.action.type) {
  case Action::TYPE_MOVE: {
    // regular movement click
    action_instance_.params.move.x = mouse_world_x_;
    action_instance_.params.move.y = mouse_world_y_;
    } break;
  case Action::TYPE_CONSUMABLE: {
      action_instance_.params.consumable.x = mouse_world_x_;
      action_instance_.params.consumable.y = mouse_world_y_;
    } break;
  case Action::TYPE_SPELL: {
      action_instance_.params.spell.x = mouse_world_x_;
      action_instance_.params.spell.y = mouse_world_y_;
      Actor::ActorID actor_id;
      if (ACTOR_SELECT_RADIUS_SQ <
           actor_manager_->getActorClosestToLocation(
              action_instance_.action.spell.spell->can_target_caster, // find client
              action_instance_.action.spell.spell->can_target_dead,   // living only
              mouse_world_x_,
              mouse_world_y_,
              &actor_id)) {
        actor_id = Evidyon::Actor::INVALID_ACTOR_ID;
      }
      action_instance_.params.spell.target_actor_id = actor_id;
      
      // save this spell
      last_targeted_spell_ = action_instance_.action.spell;
    } break;
  case Action::TYPE_ACTORINTERACTION: {
      Actor::ActorID actor_id;
      if ((ACTOR_SELECT_RADIUS_SQ >
           actor_manager_->getActorClosestToLocation(
              false,    // don't find client
              false,    // living only
              mouse_world_x_,
              mouse_world_y_,
              &actor_id)) &&  // find an actor...
          (actor_id == action_instance_.params.actor_interaction.actor_id)) { // ...that is the same one we started with
        switch (actor_manager_->getActorInteraction(actor_id)) {
          using namespace Evidyon::Actor;
          case ACTORINTERACTION_ATTACK: {
            cancelAction(); // don't processs this here anymore--we're using the action queue now!

            Action_Attack* action = Evidyon::Client::ActionQueue::add<Action_Attack>();
            action->actor_id = actor_id;
            action->x = mouse_world_x_;
            action->y = mouse_world_y_;
          } break;
          case ACTORINTERACTION_OPEN_STORAGE:
            Evidyon::Client::ActionQueue::add<Action_StorageOpen>()->npc_actor_id = actor_id;
            break;
          case ACTORINTERACTION_OPEN_BAZAAR:
            Evidyon::Client::ActionQueue::add<Action_BazaarOpen>()->npc_actor_id = actor_id;
            break;
          case ACTORINTERACTION_TRIGGER:
            Evidyon::Client::ActionQueue::add<Action_NPCTrigger>()->npc_actor_id = actor_id;
            break;
        }
        action_is_valid_ = false;
        action_instance_.action.type = Action::TYPE_INVALID;
      } else {
        action_is_valid_ = false;
        action_instance_.action.type = Action::TYPE_INVALID;
      }

      if (!action_is_valid_) { // default click action = move
        action_is_valid_ = true;
        action_needs_target_ = false;
        action_instance_.action.type = Action::TYPE_MOVE;
        action_instance_.params.move.streaming = false;
        action_instance_.params.move.x = mouse_world_x_;
        action_instance_.params.move.y = mouse_world_y_;
      }
    } break;
  }
}




//----[  isTargeting  ]--------------------------------------------------------
bool ActionTargeter::isTargeting() const {
  // get the slot before targeting
  return action_is_valid_ && (action_needs_target_ && !action_needs_slot_);
}


//----[  isReadingSlot  ]------------------------------------------------------
bool ActionTargeter::isReadingSlot() const {
  return action_is_valid_ && action_needs_slot_;
}

void ActionTargeter::userEvent(UserEventType user_event_type) {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = user_event_type;
}

#define USEREVENT_ACTION(user_event_type, variable)  \
  action_needs_target_=false;action_is_valid_=true;\
  action_instance_.action.type = Action::TYPE_USEREVENT;\
  action_instance_.action.user_event=user_event_type;\
  return &action_instance_.params.user_event.data_.##variable;


UserEventAddAbilityPoint* ActionTargeter::attributesAddAbilityPoint() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = ATTRIBUTES_ADD_ABILITY_POINT;
  return &action_instance_.params.user_event.data_.add_ability_point_;
}

UserEventOnItem* ActionTargeter::dropItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = DROP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventDropCurrency* ActionTargeter::dropCurrency() { USEREVENT_ACTION(DROP_CURRENCY, drop_currency_); }

UserEventOnItem* ActionTargeter::equipItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = EQUIP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::unequipItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = UNEQUIP_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}


UserEventOnItem* ActionTargeter::storageWithdrawItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = STORAGE_WITHDRAW_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::storageDepositItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = STORAGE_DEPOSIT_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::geosidSacrificeItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = GEOSID_SACRIFICE_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::tradeAddItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = TRADE_ADDITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventOnItem* ActionTargeter::tradeRemoveItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = TRADE_REMOVEITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}

UserEventTradeSetCurrency* ActionTargeter::tradeSetCurrency() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = TRADE_SETCURRENCY;
  return &action_instance_.params.user_event.data_.trade_set_currency_;
}

UserEventOnItem* ActionTargeter::splitStackedItem() {
  action_needs_target_ = false;
  action_is_valid_ = true;
  action_instance_.action.type = Action::TYPE_USEREVENT;
  action_instance_.action.user_event = SPLIT_STACKED_ITEM;
  return &action_instance_.params.user_event.data_.item_event_;
}



UserEventQuestOfferResponse* ActionTargeter::questOfferResponse() { USEREVENT_ACTION(QUEST_OFFER_RESPONSE, quest_offer_response_); }
UserEventQuestGiveUp* ActionTargeter::questGiveUp() { USEREVENT_ACTION(QUEST_GIVE_UP, quest_give_up_); }


#undef USEREVENT_ACTION






//----[  consumeItem  ]--------------------------------------------------------
void ActionTargeter::consumeItem(int inventory_index,
                                 bool needs_target,
                                 bool needs_slot) {
  action_instance_.action.type = Action::TYPE_CONSUMABLE;
  action_instance_.action.consumable.item_type = -1; // unknown
  action_instance_.params.consumable.inventory_index_hint = inventory_index;
  action_instance_.params.consumable.x = mouse_world_x_;
  action_instance_.params.consumable.y = mouse_world_y_;
  action_needs_target_ = needs_target;
  action_needs_slot_ = needs_slot;
  action_is_valid_ = true;
}


//
//void ActionTargeter::npcInteraction(Evidyon::Actor::ActorID npc_actor_id, NPCInteractionType interaction_type) {
//  action_instance_.action.type = Action::TYPE_NPCINTERACTION;
//  action_instance_.params.npc_interaction.npc_actor_id = npc_actor_id;
//  action_instance_.params.npc_interaction.interaction_type = interaction_type;
//  action_needs_target_ = false;
//  action_is_valid_ = true;
//}



//----[  unstickMouse  ]-------------------------------------------------------
void ActionTargeter::unstickMouse() {
  mouse_stuck_ = false;
  camera_->getWorldLocation(last_mouse_position_.x, last_mouse_position_.y,
                           &mouse_world_x_, &mouse_world_y_);
}



//----[  castSpell  ]----------------------------------------------------------
void ActionTargeter::castSpell(Spell::SpellIndex type, const Spell::SpellClientDescription* description) {
  action_instance_.action.type = Action::TYPE_SPELL;
  action_instance_.action.spell.spell_type = type;
  action_instance_.action.spell.spell = description;
  action_instance_.params.spell.x = mouse_world_x_;
  action_instance_.params.spell.y = mouse_world_y_;
  action_needs_target_ = description->needs_target;
  action_needs_slot_ = description->needs_slot;
  action_is_valid_ = true;
}



//----[  onNumberKey  ]--------------------------------------------------------
bool ActionTargeter::onNumberKey(int number) {
  if (action_is_valid_ && action_needs_slot_) {
    if (action_instance_.action.type == Action::TYPE_SPELL) {
      action_instance_.params.spell.slot = number;
      action_needs_slot_ = false;
    } else if (action_instance_.action.type == Action::TYPE_CONSUMABLE) {
      action_instance_.params.consumable.slot = number;
      action_needs_slot_ = false;
    } else {
      assert(false); // don't know what to do with this
      return false;
    }
    return true;
  }
  return false;
}




//----[  update  ]-------------------------------------------------------------
Action::Type ActionTargeter::update(double time, const ActionInstance** action_instance) {
  last_update_time_ = time;
  if (time > unstick_mouse_time_) {
    mouse_stuck_ = false;
  }
  if (!mouse_stuck_) {
    camera_->getWorldLocation(last_mouse_position_.x,
                              last_mouse_position_.y,
                             &mouse_world_x_,
                             &mouse_world_y_);
  }

  if (action_is_valid_) {
    if (action_instance_.action.type == Action::TYPE_ACTORINTERACTION &&
        !mouse_stuck_) {
      action_is_valid_ = false;
      action_needs_target_ = false;
      return_movement_ = true;
      next_time_to_return_movement_ = 0.0;
    }
    if (action_needs_target_ || action_needs_slot_) {
      return Action::TYPE_INVALID; // don't do anything yet
    }
    action_is_valid_ = false;
  } else { // return a movement action
    bool clicked = true;
    if (return_movement_) {
      if (next_time_to_return_movement_ > time) return Action::TYPE_INVALID;
      next_time_to_return_movement_ = time + MOVEMENT_STREAM_PERIOD;
      if (mouse_stuck_) {
        Actor::ActorID actor_id;
        if (ACTOR_SELECT_RADIUS_SQ > actor_manager_->getActorClosestToLocation(
              false,
              false,
              mouse_world_x_,
              mouse_world_y_,
              &actor_id)) {
          next_time_to_return_movement_ = -1.0; // be sure to do this!
          return_movement_ = false;
          action_is_valid_ = true;
          action_needs_target_ = true;
          action_instance_.action.type = Action::TYPE_ACTORINTERACTION;
          action_instance_.params.actor_interaction.actor_id = actor_id;
          return Action::TYPE_INVALID;
        } else {
          // wait until the mouse is unstuck to send the next message
          // in order to prevent "bouncing" (backtracking)
          next_time_to_return_movement_ = time + MOVEMENT_STREAM_PERIOD + MOUSE_STICK_TIME;
        }
      }

      clicked = false; // streaming-movement mode:  don't attack anything
      action_instance_.action.type = Action::TYPE_MOVE;
      action_instance_.params.move.x = mouse_world_x_;
      action_instance_.params.move.y = mouse_world_y_;
      action_instance_.params.move.streaming = true;

    } else {

      // check to see if the final movement message has been sent to the
      // server to terminate streaming movement
      if (next_time_to_return_movement_ <= 0.0) return Action::TYPE_INVALID;

      // If we get here, the mouse was JUST released after having been
      // streaming--so we should send a final movement message to cancel
      // out the streaming.
      next_time_to_return_movement_ = -1.0;
    }

    if (clicked) { // the mouse was clicked; set up a "go here" message
      action_instance_.action.type = Action::TYPE_MOVE;
      action_instance_.params.move.x = mouse_world_x_;
      action_instance_.params.move.y = mouse_world_y_;
      action_instance_.params.move.streaming = false;
    }
  }

  // A valid action is being returned, so define its type
  *action_instance = &action_instance_;
  return action_instance_.action.type;
}


}