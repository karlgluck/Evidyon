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
#ifndef __ACTORGROUP_H__
#define __ACTORGROUP_H__
#pragma once


#include "shared/networkdecl.h"
#include "shared/avatar/avatarlevel.h"
#include "shared/avatar/avatarexperience.h"

namespace Evidyon {

class ActorInstance;
class Client;

// An actor group is a *SMALL* collection of actors: a raid party, for example
class ActorGroup {
public:
  static const int MAX_MEMBERS = 8;

public:
  ActorGroup();

  unsigned __int64 getPartyID();

  int findMembersNear(ActorInstance* source, float radius_squared,
                      ActorInstance** actors, int buffer_size);

  // Returns actors that are enemies of 'enemies_of_actor' that are near (x,y)
  // and are not members of this group.
  int findEnemiesNear(ActorInstance* enemies_of_actor, float x, float y,
                      float radius_squared, ActorInstance** actors,
                      int buffer_size);

  bool isGroupMember(ActorInstance* actor) const;

  // If a group is open, new members can join
  void setGroupOpen(bool is_open);

  // Tries to put a new member in this group.  Returns 'true' if the member was
  // successfully added.  A member can't be added to a group that is full.
  // If removeMember erases the last entry in the group, it will deallocate that
  // group--so after calling removeMember, it is not valid to call any other
  // method on this group!
  // These methods do NOT call ActorInstance::setGroup(), since that method invokes
  // these ones!
  bool addMember(ActorInstance* actor);
  void removeMember(ActorInstance* actor);

  // Sends a packet on the secondary channel to each client whose actor is a member
  // of this class.
  void sendToClients(NetworkPacket* packet);

  // Distributes exp gained by one character among all nearby clients in this group
  void shareMonsterEXP(ActorInstance* source,
                       Avatar::AvatarLevel monster_intended_level,
                       Avatar::AvatarExperience amount);

  // Party member has killed someone, so activate PK for clients
  void partyMemberPKed(ActorInstance* killer_actor,
                       Client* killer_client,
                       Client* killed);

  bool kickMember(const char* name);

public:

  // Allocates a new group and adds the given actor as its founder.  The group will
  // be erased when it has no more members.  This method sets the founder's group
  // to be the newly created group automatically.
  static ActorGroup* createGroup(ActorInstance* founder);

  // Adds the follower to the leader's group.  If the leader doesn't have an active
  // ActorGroup, this method creates one.  If the return value is 'false', neither
  // actors' groups will have changed.  'true' indicates the follower successfully
  // joined the leader's group.
  static bool joinGroup(ActorInstance* actor_leader, ActorInstance* actor_follower);


private:
  void recalculateEXPShare();


private:
  ActorInstance* members_[MAX_MEMBERS];
  int number_of_members_;

  // values used to share experience
  int sum_of_levels_squared_;

  // New members can only be added if the group is open.  Groups
  // default to being open.
  bool is_open_;

  // generated automatically when the group forms
  unsigned __int64 party_id_;
};

}

#endif