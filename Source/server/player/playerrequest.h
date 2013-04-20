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
#ifndef __SERVER_PLAYER_PLAYERREQUEST_H__
#define __SERVER_PLAYER_PLAYERREQUEST_H__
#pragma once

#include "shared/actor/actorid.h"
#include "shared/guild/guildlimits.h"
#include "common/number.h"
#include "shared/avatar/avatarlimits.h"
#include "common/kg/kg.h"
#include "shared/geosid/geonite.h"

namespace Evidyon {
struct SpellDescription;
}

namespace Evidyon {
namespace Server {


//----[  PlayerRequest  ]------------------------------------------------------
// These are things that the player behind the remote client has asked be done
// by their avatar.
struct PlayerRequest {
  // These are in priority order; first is highest priority.
  enum Type {
    LEAVE_WORLD,  // exit the game world
    STANDUP,      // dead player wants to get up
    RESPAWN,      // dead player wishes to go home
    STOP_ACTION,
    //GEOSID_OPEN,  // actor is pre-validated near geosid
    STORAGE_OPEN, // actor is pre-validated near storage npc
    BAZAAR_OPEN,  // actor is pre-validated near bazaar npc
    USE_ITEM,
    CAST,
    GEOSID_BURST, // use geosid burst magic
    ATTACK,
    MOVE,
    SPEAK,        // if phrase is "trade" will activate trade logic
    CREATE_GUILD, // if player joins, guild is created; do not reset!
    LEAD_GUILD,   // wait for players to join guild
    NONE,         // request processed
  } type;

  // Each network message that would change the player request should call
  // this method with its corresponding type.  If the return value is false,
  // the message should be ignored since a higher priority message
  // is pending.
  bool changeTo(Type new_type)
    { if (new_type <= type) { type = new_type; return true; } return false; }

  // Call this once a request has been processed
  void reset() {
    if (type == SPEAK) speak.text[0] = '\0';
    type = speak.text[0] == '\0' ? NONE : SPEAK;
  }

  // Call this to reset the request if it is of the provided type
  void reset(Type type_to_reset) {
    if (type == type_to_reset) reset();
  }

  union {
    struct {
      NumberNoConstructor x, z;
      Evidyon::Actor::ActorID actor_id;
      unsigned int inventory_index;
      unsigned int parameter;
    } use_item;

    struct {
      NumberNoConstructor x, z;
      Evidyon::Actor::ActorID actor_id;
      const Evidyon::SpellDescription* spell;
      unsigned int parameter;
    } cast;

    struct {
      NumberNoConstructor x, z;
      Evidyon::Actor::ActorID actor_id;
      unsigned int parameter;
      const Evidyon::Magic::Magic* geosid_magic;
      Geosid::Geonite geonite_cost;
    } geosid_burst;

    struct {
      Evidyon::Actor::ActorID actor_id;
    } attack;

    struct {
      NumberNoConstructor x, z;
      bool streaming;
    } move;

    struct {
      char guild_name[Guild::MAX_NAME_LENGTH];
    } create_guild;
  };

  // This type is special--speech is preserved though all other high priority
  // requests and is processed whenever those are done.
  struct {
    kg::CharBuffer<Avatar::MAX_SPOKEN_TEXT_LENGTH+1> text;
  } speak;
};


}
}

#endif