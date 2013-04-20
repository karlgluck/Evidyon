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
#ifndef __CLIENT_ACTOR_CLIENTACTORINSTANCE_H__
#define __CLIENT_ACTOR_CLIENTACTORINSTANCE_H__
#pragma once


#include "shared/actor/actorid.h"
#include "shared/actor/actoralignment.h"
#include <string>
#include "shared/binding/bindingptr.h"
#include "shared/guild/guildlimits.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/magic/enchantmenttypeindex.h"
#include "shared/event/eventid.h"


namespace Evidyon {
namespace Actor {
struct RenderableActorInstance;
}
}

namespace Evidyon {
namespace Actor {
namespace Client {


  
//----[  ClientActorInstance  ]------------------------------------------------
struct ClientActorInstance {

  // Rolls the next spoken phrase into the currently displayed text.  This
  // method is called whenever the currently displayed speech text expires
  // so that the next phrase is put into the buffer.
  // If this method changed the current phrase, it returns 'true'.
  bool setNextSpokenPhrase(double time);

  // If the actor is speaking to the avatar of the person playing the
  // game (noted by 'important' flag), it will clear anything the actor is
  // currently saying and the speech can't be interrupted until it finishes,
  // except by another important setSpeech.
  void setSpeech(bool important, const std::string& text);

  // Obtains the location of the actor in the world
  void getPosition(float* x, float* z) const;

  // Obtains this actor's alignment
  Actor::ActorAlignment getAlignment() const;

  // Obtains the actor's binding
  Binding::BindingPtr getBinding();

  // Resets all values
  void zero();

public:

  // The actor's own unique ID
  ActorID id;

  // Rendering object used for this actor
  RenderableActorInstance* renderable_instance;

  // What this actor is saying, and when to remove the speech.  This structure
  // allows actors saying large amounts of text (such as NPCs giving a quest)
  // to send it all at once.  The text will then be phrased automatically.
  struct {
    std::string displayed_text[2], remaining_text;

    // which displayed text line should come first changes; uses these to index
    // that line.
    int current_line, last_line;

    // used to crossfade the two text lines
    float displayed_text_fade[2];

    // for the two lines of displayed text, add line height * this value to the
    // place where one line would normally be displayed (ranges from -1->0)
    float displayed_text_top_line_offset;

    // Time at which to rotate in the next phrase from remaining_text, or clear
    // the displayed text if there is nothing more to say.
    double expiration_time;

    // Important text is not overwritten by text that does not have the
    // 'important' flag set.  When an NPC directs speech at the actor that
    // represents the client's avatar, it is marked as important.
    bool important;
  } speech;

  // This value is used to make non-client actors disappear when they are no
  // longer being synced by the server.
  unsigned int slow_sync_counter;

  // If this client is a member of a guild, this value is set
  char guild_name[Guild::MAX_GUILD_MEMBER_NAME_TAG_LENGTH];

  struct {
    Magic::EnchantmentTypeIndex type;
    Event::EventID active_event_id;
  } enchantments[Magic::NUMBER_OF_ENCHANTMENT_TYPES];
};



}
}
}

#endif