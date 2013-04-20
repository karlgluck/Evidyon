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
#include "actorgroup.h"
#include "actor.h"
#include "../common/null.h"
#include "globalactormanager.h"
#include "worldregion.h"
#include <dc/debug>

// for sending group messages
#include "encodepackets.h"
#include "globalnetworkmanager.h"

#include "server/main.h"

namespace Evidyon {



  
//----[  ActorGroup  ]-------------------------------------------------------
ActorGroup::ActorGroup() {
  number_of_members_ = 0;
  for (int i = 0; i < MAX_MEMBERS; ++i) {
    members_[i] = NULL;
  }
  sum_of_levels_squared_ = 0;
  is_open_ = true;

  static Unique64BitGenerator party_id_generator;
  party_id_generator.next(&party_id_);
}




//----[  getPartyID  ]---------------------------------------------------------
unsigned __int64 ActorGroup::getPartyID() {
  return party_id_;
}


//----[  findMembersNear  ]----------------------------------------------------
int ActorGroup::findMembersNear(ActorInstance* source, float radius_squared,
                ActorInstance** actors, int buffer_size) {

  // Loop through all group members that are not the source and pull out the
  // ones that are within the rage of the source actor.
  int members_found = 0;
  for (int i = 0; i < number_of_members_ && members_found < buffer_size; ++i) {
    ActorInstance* actor = members_[i];
    if (actor == source) continue;
    if (actor->distanceToSq(source) < radius_squared) {
      *(actors++) = actor;
      ++members_found;
    }
  }
  return members_found;
}



//----[  findEnemiesNear  ]----------------------------------------------------
int ActorGroup::findEnemiesNear(ActorInstance* enemies_of_actor, float x, float y,
                float radius_squared, ActorInstance** actors,
                int buffer_size) {
  // This temporary buffer is large enough to include the full set of group
  // members, just in case the group is mixed with non-group enemies
  // and some members of the group are normally enemies of each other.
  ActorInstance* temp[MAX_MEMBERS + GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE];
  int actors_to_find = MAX_MEMBERS +
    buffer_size > GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE ?
    GlobalActorManager::STD_ACTOR_SEARCH_BUFFER_SIZE : buffer_size;

  int actors_found =
    enemies_of_actor->getRegion()->findActorEnemiesNearby(enemies_of_actor,
                                                          x,
                                                          y,
                                                          radius_squared,
                                                          temp,
                                                          actors_to_find);

  // Copy non-group members into the buffer
  int enemies_found = 0;
  for (int i = 0; i < actors_found && enemies_found < buffer_size; ++i) {
    if (temp[i]->getGroup() != this) {
      *(actors++) = temp[i];
      ++enemies_found;
    }
  }

  return enemies_found;
}




//----[  isGroupMember  ]------------------------------------------------------
bool ActorGroup::isGroupMember(ActorInstance* actor) const {
  return actor != NULL && actor->getGroup() == this;
}




//----[  setGroupOpen  ]-------------------------------------------------------
void ActorGroup::setGroupOpen(bool is_open) {
  is_open_ = is_open;
}





//----[  addMember  ]----------------------------------------------------------
bool ActorGroup::addMember(ActorInstance* actor) {
  if (!is_open_ || number_of_members_ >= MAX_MEMBERS) return false;
  members_[number_of_members_++] = actor;
  recalculateEXPShare();

  // Success
  return true;
}





//----[  removeMember  ]-------------------------------------------------------
void ActorGroup::removeMember(ActorInstance* actor) {
  if (actor->getGroup() != this) return;

  for (int i = 0; i < number_of_members_; ++i) {
    if (members_[i] == actor) {
      --number_of_members_;
      if (number_of_members_ > i) {
        members_[i] = members_[number_of_members_];
      }
      members_[number_of_members_] = NULL;
      break;
    }
  }

  if (number_of_members_ == 0) {
    delete this;
  } else {
    recalculateEXPShare();
  }
}





//----[  sendToClients  ]------------------------------------------------------
void ActorGroup::sendToClients(NetworkPacket* packet) {
  for (int i = 0; i < number_of_members_; ++i) {
    Evidyon::Client* client = members_[i]->getController()->getClient();
    if (client == NULL) continue;
    client->sendAsyncPacket(packet);
  }
}




//----[  shareMonsterEXP  ]----------------------------------------------------
void ActorGroup::shareMonsterEXP(ActorInstance* source,
                                 Avatar::AvatarLevel monster_intended_level,
                                 Avatar::AvatarExperience amount) {
  for (int i = 0; i < number_of_members_; ++i) {
    ActorInstance* member = members_[i];
    if (member != source && (member->distanceToSq(source) >= 16.0f * 16.0f)) continue;
    Evidyon::Client* client = members_[i]->getController()->getClient();
    if (client == NULL) continue;
    client->shareMonsterEXP(
      amount,
      monster_intended_level,
      sum_of_levels_squared_);
  }
}


//----[  partyMemberPKed  ]----------------------------------------------------
void ActorGroup::partyMemberPKed(ActorInstance* killer_actor,
                                 Client* killer_client,
                                 Client* killed) {
  for (int i = 0; i < number_of_members_; ++i) {
    ActorInstance* member = members_[i];
    if (member == killer_actor ||
        (member->distanceToSq(killer_actor) >= 16.0f * 16.0f)) continue;

    // don't doubly hit
    Evidyon::Client* client = members_[i]->getController()->getClient();
    if (client == NULL) continue;
    if (killed->attackedBy(client) == false) {
      client->onPK();
    }
  }
}




//----[  kickMember  ]---------------------------------------------------------
bool ActorGroup::kickMember(const char* name) {
  for (int i = 0; i < number_of_members_; ++i) {
    ActorInstance* member = members_[i];
    assert(member);
    Evidyon::Client* client = member->getController()->getClient();
    if (client == NULL) continue;
    if (stricmp(name,client->getName()) == 0) {
      member->setGroup(NULL); // erase the member
      return true;
    }
  }
  // couldn't find the member
  return false;
}


//----[  createGroup  ]--------------------------------------------------------
ActorGroup* ActorGroup::createGroup(ActorInstance* founder) {
  ActorGroup* group = new ActorGroup();
  founder->setGroup(group);
  return group;
}





//----[  joinGroup  ]----------------------------------------------------------
bool ActorGroup::joinGroup(ActorInstance* actor_leader, ActorInstance* actor_follower) {
  Evidyon::ActorGroup* group = actor_leader->getGroup();
  if (group == NULL) {
    ASSERT(createGroup(actor_leader)->addMember(actor_follower));
    return true;
  } else {
    return group->addMember(actor_follower);
  }
}





//----[  recalculateEXPShare  ]------------------------------------------------
void ActorGroup::recalculateEXPShare() {
  int sum_of_squares = 0;

  for (int i = 0; i < number_of_members_; ++i) {
    ActorInstance* member = members_[i];
    ActorController* actor_controller = member->getController();
    Client* client = actor_controller->getClient();
    if (client == NULL) continue;
    int level = client->getLevel();
    sum_of_squares += level * level;
  }

  sum_of_levels_squared_ = sum_of_squares;
}



}