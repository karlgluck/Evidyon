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
#ifndef __ACTORDESCRIPTIONTEMPLATE_H__
#define __ACTORDESCRIPTIONTEMPLATE_H__
#pragma once



// An actor template is compiled by the editor for every kind of actor; the
// index of the template is encoded into each actor's ID number so that the
// client can implicitly determine what that actor looks like and the basic
// character attributes without requiring a full description.
// In fact, most actors don't require a description at all: a scorpion is
// always named "scorpion", has no items, no enchantments, and always uses
// the scorpion model.  Because of this, the client doesn't have to request
// a description for the model--ever!

// contributors:
//  1- character races, genders
//  2- monsters

//
//actor types:
// - have the editor compiled actor "template" types.  these are essentially the default
//   complete descriptions to use for an actor.  an actor's id % num_types gives its
//   template type (the other bits are for making the id unique).
// - most actors don't modify their base type, so sending the description is "free" if
//   types are templated--it's implied in the slowsync update!
// - the description includes name, equipped items, enchantments, action speeds, etc.
// - description settings include whether or not the actor's name, items, enchantments
//   will ever change. the client only requests updates if change is possible.
//
//since the total number of actors in the world is limited, the range of IDs is limited
//
//actor ID:
//    - 9 to 11 bits derived from index in actor memory pool
//      (guaranteed unique at a point in time!)
//    - 10 bits for actor template type
//    - the rest of the bits are for a random mask to make sure that actors dont get
//      confused


#include "evidyon_actor.h"


namespace Evidyon {
//
//static const int MAX_ACTOR_NAME_LENGTH = 32;
//
//enum ActorTemplateType {
//  ACTORTEMPLATETYPE_PLAYER,
//  ACTORTEMPLATETYPE_MONSTER,
//  ACTORTEMPLATETYPE_NPC_STORAGE,
//  ACTORTEMPLATETYPE_NPC_BAZAAR,
//  ACTORTEMPLATETYPE_NPC_GEOSID,
//  ACTORTEMPLATETYPE_NPC_MERCHANT,
//  ACTORTEMPLATETYPE_NPC_QUESTGIVER,
//};
//
//struct ClientActorDescriptionTemplate {
//  int actor_type; // the animated mesh to use
//  char name[MAX_ACTOR_NAME_LENGTH]; // if this has no length, request the name from the server
//  //int equipment[SLOTS];
//  unsigned int enchantments;  // the full enchantment mask
//
//  // If anything in this description can be changed, this flag is
//  // set so that the client can send a request for this actor's
//  // updated description to the server.
//  bool description_can_change;
//
//  // These speeds provide a baseline for this type.  If
//  // the speeds can change, the server provides a scaling
//  // factor by which all of these values are multiplied.
//  float action_speeds[ACTORACTION_VARIABLE_SPEED_COUNT];
//
//  // Helps define what should occur when the player clicks on this actor
//  ActorTemplateType template_type;
//
//  // Defines equipment
//  unsigned int equipped_item_scenery[ACTORATTACHMENTPOINT_COUNT];
//
//  // The default alignment type for this actor
//  Evidyon::Actor::ActorAlignment alignment;
//};
//
//
//
//// If a client requests an actor's description, this structure
//// defines what information needs to be sent to the client in order
//// to update it completely.
//struct ServerActorDescriptionTemplate {
//  int actor_type; // the animated mesh to use
//  float radial_size;
//};

}



#endif