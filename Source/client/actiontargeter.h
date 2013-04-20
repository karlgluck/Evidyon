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
#ifndef __ACTIONTARGETER_H__
#define __ACTIONTARGETER_H__
#pragma once


#include "shared/actor/actorid.h"
#include "useractionqueue.h"
#include "common/gui/guimouse.h"
#include "shared/evidyon_npc.h"
#include "shared/spell/spellindex.h"
#include "shared/item/itemindex.h"
#include "shared/geosid/geosidindex.h"


namespace Evidyon {
namespace Actor {
namespace Client {
enum ClientActorInteraction;
class ClientActorManager;
}
}
namespace Spell {
struct SpellClientDescription;
}
}

class IsometricCamera;

namespace Evidyon {

class GlobalActorManager;

struct OldAction_CastSpell {
  Evidyon::Spell::SpellIndex spell_type;
  const Evidyon::Spell::SpellClientDescription* spell;
  struct Parameters {
    Evidyon::Actor::ActorID target_actor_id;
    float x, y;
    int slot;
  };
};

struct OldAction_UseConsumable {
  Evidyon::Item::ItemIndex item_type;
  struct Parameters {
    int inventory_index_hint;
    bool needs_target;
    float x, y;
    int slot;
  };
};

struct OldAction_Move {
  struct Parameters {
    bool streaming;
    float x, y;
  };
};

struct OldAction_ActorInteraction {
  struct Parameters {
    Evidyon::Actor::ActorID actor_id;
  };
};

struct OldAction_Attack {
  struct Parameters {
    Evidyon::Actor::ActorID actor_id;
    float x, y;
  };
};

struct OldAction_NPCInteraction {
  struct Parameters {
    Evidyon::Actor::ActorID npc_actor_id;
    NPCInteractionType interaction_type;
  };
};

struct OldAction_AlternateAction {
  struct Parameters {
    Evidyon::Actor::ActorID actor_id;
    float x, y;
  };
};


struct Action {
  enum Type {
    TYPE_INVALID,
    TYPE_USEREVENT,
    TYPE_SPELL,
    TYPE_CONSUMABLE,
    TYPE_MOVE,
    TYPE_ACTORINTERACTION, // when the mouse goes up, this is parsed into nothing/attack/npc types
    TYPE_ATTACK,
    TYPE_ALTERNATEACTION, // right-click action
  } type;
  union {
    // can be bound to keys
    UserEventType user_event;
    OldAction_CastSpell spell;
    OldAction_UseConsumable consumable;
  };
};




//----[  ActionInstance  ]-----------------------------------------------------
struct ActionInstance {
  Action action;
  union {
    UserEvent user_event;
    OldAction_CastSpell::Parameters spell;
    OldAction_UseConsumable::Parameters consumable;
    OldAction_Move::Parameters move;
    OldAction_ActorInteraction::Parameters actor_interaction;
    OldAction_Attack::Parameters attack;
    OldAction_NPCInteraction::Parameters npc_interaction;
    OldAction_AlternateAction::Parameters alternate_action;
  } params;
};




//----[  ActionTargeter  ]-----------------------------------------------------
class ActionTargeter : public GUIMouseSentinel {
public:
  ActionTargeter();
  void create(IsometricCamera* camera,
              Actor::Client::ClientActorManager* actor_manager);

  virtual bool onMouseMove(GUIPoint position, GUIPoint oldPosition, GUIMouseInputSource* mouse);
  virtual bool onMouseButtonAction(GUIMouseButtonAction action, GUIPoint position, GUIMouseInputSource* mouse);

  // Advances the targeter.  The returned value is a reference to the action
  // the user wishes to perform.
  Action::Type update(double time, const ActionInstance** OldAction_instance);

  bool isTargeting() const;
  bool isReadingSlot() const;

  void userEvent(UserEventType user_event_type);
  UserEventAddAbilityPoint* attributesAddAbilityPoint();
  UserEventSpeak* speak();
  UserEventOnItem* dropItem();
  UserEventDropCurrency* dropCurrency();
  UserEventOnItem* equipItem();
  UserEventOnItem* unequipItem();
  UserEventOnItem* storageWithdrawItem();
  UserEventOnItem* storageDepositItem();
  UserEventOnItem* geosidSacrificeItem();


  UserEventOnItem* tradeAddItem();
  UserEventOnItem* tradeRemoveItem();
  UserEventTradeSetCurrency* tradeSetCurrency();

  UserEventOnItem* splitStackedItem();

  UserEventQuestOfferResponse* questOfferResponse();
  UserEventQuestGiveUp* questGiveUp();

  void consumeItem(int inventory_index, bool needs_target, bool needs_slot);
  void castSpell(Spell::SpellIndex type, const Spell::SpellClientDescription* description);

  // User typed a number {0, 1, 2, ..., 9}.  Used to select portal destination
  // and other such "slotted" actions.  Returns 'true' if the keypress was consumed.
  bool onNumberKey(int number);

  void npcInteraction(Evidyon::Actor::ActorID npc_actor_id, NPCInteractionType interOldAction_type);

  void unstickMouse();

private:
  // Called when the user wants to perform an action.  This either feeds straight
  // into the update() output, or blocks waiting for a target.
  void beginAction(const Action* action);

  // Gets rid of the current action, if it is valid
  bool cancelAction();

  // If action_is_valid_, defines the action target with the current mouse world
  // coordinates and clears action_needs_target_
  void setActionTarget(bool is_geonite_burst);


private:
  double last_update_time_;

  // This value is returned via update().  It is filled out either by an explicit
  // command from the user, by movement, or by nothing.
  ActionInstance action_instance_;
  bool action_is_valid_;      // begin() has been called
  bool action_needs_target_;  // action_is_valid_ == true, and we're waiting for a target
  bool action_needs_slot_;    // action_is_valid_ == true, and we're waiting for a slot (# 0-9)

  // the spell to cast next time the player right-clicks
  OldAction_CastSpell last_targeted_spell_;

  // Used to "stick" the mouse position and keep it from updating
  // for a short period when it is first pressed
  double unstick_mouse_time_;
  bool mouse_stuck_;
  float mouse_world_x_, mouse_world_y_;
  GUIPoint last_mouse_position_;

  // movement is returned periodically when the primary mouse button is down
  bool return_movement_;
  double next_time_to_return_movement_;

  // if both the primary and secondary buttons are held, this list contains a stream
  // that takes the user to the location at which this condition first occurred
  //std::list<std::pair<float, float>> movement_path_;

  // Translates screen->world coordinates
  IsometricCamera* camera_;

  // Finds actors that the player clicked on to provide interaction
  Actor::Client::ClientActorManager* actor_manager_;
};


}


#endif