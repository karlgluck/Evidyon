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
#include "clientactorinstance.h"
#include "shared/actor/renderableactorinstance.h"
#include "shared/binding/binding.h"
#include "shared/binding/actorbinding.h"
#include <assert.h>

namespace Evidyon {
namespace Actor {
namespace Client {



static const int PHRASE_SPOKEN_TEXT_CHARACTERS = 44;
static const double SPEECH_EXPIRATION_DELAY = 4.0;
static const double SPEECH_PHRASE_DELAY = 3.0;



  
//----[  setNextSpokenPhrase  ]------------------------------------------------
bool ClientActorInstance::setNextSpokenPhrase(double time) {
  if (speech.displayed_text[speech.current_line].empty() &&
      speech.remaining_text.empty()) return false;
  if (speech.displayed_text[speech.last_line].empty() == false) {
    // decreases with time from 2->0
    float fade = 2 * (speech.expiration_time - time) / SPEECH_EXPIRATION_DELAY;
    fade = 2 - fade; // 0 -> 2
    if (fade > 1.0) fade = 1.0; // 0 -> 1
    speech.displayed_text_fade[speech.last_line] = 1.0 - fade; // 1->0
    float slide = - 8 * fade; // 0 -> -8
    if (slide < -1.0) slide = -1.0; // 0 -> -1
    // slide much faster than fade speed
    speech.displayed_text_top_line_offset = slide;
  }
  if (speech.expiration_time >= time) return false;
  if (speech.remaining_text.empty()) {
    // this final fade is based on time being later than the expiration time
    float fade = 1.0 - (time - speech.expiration_time) / SPEECH_EXPIRATION_DELAY;
    if (fade <= 0.0) {
      // done fading out
      speech.displayed_text[speech.current_line].clear();
      speech.displayed_text[speech.last_line].clear();
      speech.expiration_time = 10000000000.0; // huge
      speech.important = false;
      return false;
    } else {
      speech.displayed_text[speech.last_line].clear();
      speech.displayed_text_fade[speech.current_line] = fade;
      return false;
    }
  } else if (speech.remaining_text.length() <= PHRASE_SPOKEN_TEXT_CHARACTERS) {
    speech.displayed_text[speech.last_line].swap(
      speech.displayed_text[speech.current_line]);
    speech.displayed_text[speech.current_line].swap(
      speech.remaining_text);
    speech.remaining_text.clear();
    speech.expiration_time = time + SPEECH_EXPIRATION_DELAY;
    speech.displayed_text_fade[speech.last_line] = 
      speech.displayed_text_fade[speech.current_line];
    speech.displayed_text_fade[speech.current_line] = 1.0;
  } else {
    // speak in phrases until finish
    speech.expiration_time = time + SPEECH_PHRASE_DELAY;

    // Get the next phrase to say by searching backward through
    // the set of speech.  If we reach the start without finding
    // a space, the whole block of unspaced jibberish is used.
    const char* text_start = speech.remaining_text.c_str();
    size_t first_character = 0;
    while (*text_start == ' ') {
      ++text_start;
      ++first_character;
    }
    const char* text_end = text_start + PHRASE_SPOKEN_TEXT_CHARACTERS;
    while (*text_end != ' ') {
      --text_end;
      if (text_end <= text_start) {
        text_end = text_start + PHRASE_SPOKEN_TEXT_CHARACTERS;
        break;
      }
    }
    speech.displayed_text_fade[speech.last_line] = 1.0;
    speech.displayed_text_fade[speech.current_line] = 1.0;
    speech.displayed_text[speech.last_line] =
      speech.displayed_text[speech.current_line];
    speech.displayed_text[speech.current_line].assign(
      speech.remaining_text.substr(first_character, text_end - text_start));
    speech.remaining_text.erase(0, first_character + text_end - text_start + 1);
  }

  if (speech.displayed_text[speech.last_line].empty()) {
    // fading line is 1st, but empty--this puts this line at 0
    speech.displayed_text_top_line_offset = -1.0;
  } else {
    // start the slide up
    speech.displayed_text_top_line_offset = 0.0;
  }
  // the phrase changed
  return true;
}



//----[  setSpeech  ]----------------------------------------------------------
void ClientActorInstance::setSpeech(bool important,
                                    const std::string& text) {
  if (speech.important && !important) return;
  speech.important = important;
  //speech.displayed_text[0].clear();
  //speech.displayed_text[1].clear();
  if (speech.displayed_text_fade[speech.current_line] < 1.0) {
    speech.displayed_text[speech.current_line].clear();
  }
  speech.displayed_text[speech.last_line].clear();
  speech.remaining_text = text;
  speech.expiration_time = 0.0;
}

  
//----[  getPosition  ]--------------------------------------------------------
void ClientActorInstance::getPosition(float* x, float* z) const {
  if (!renderable_instance) {
    assert(false);
    *x = -999.0f;
    *z = -999.0f;
    return;
  }
  *x = renderable_instance->current_movement_state.x;
  *z = renderable_instance->current_movement_state.y;
}


//
////----[  isEraseable  ]--------------------------------------------------------
//bool ClientActorInstance::isEraseable(double time) const {
//  float time_since_last_sync
//    = time - renderable_instance->last_sync.time_received;
//  return time_since_last_sync > 1.5;
//    //> (renderable_instance->skinned_mesh_instance->hidden ? 1.0 : 4.0);
//}



//----[  getAlignment  ]-------------------------------------------------------
Actor::ActorAlignment ClientActorInstance::getAlignment() const {
  return renderable_instance->base.alignment;
}




//----[  getBinding  ]---------------------------------------------------------
Binding::BindingPtr ClientActorInstance::getBinding() {
  return std::tr1::static_pointer_cast<
    Binding::Binding,
    Binding::ActorBinding>(renderable_instance->binding);
}


//----[  zero  ]---------------------------------------------------------------
void ClientActorInstance::zero() {
  id = Actor::INVALID_ACTOR_ID;
  renderable_instance = NULL;
  speech.displayed_text[0].clear();
  speech.displayed_text[1].clear();
  speech.displayed_text_fade[0] = 1.0;
  speech.displayed_text_fade[1] = 0.0;
  speech.current_line = 0;
  speech.last_line = 1;
  speech.displayed_text_top_line_offset = 0.0;
  speech.remaining_text.clear();
  speech.expiration_time = 0;
  speech.important = false;
  slow_sync_counter = 0;
  guild_name[0] = '\0';
  for (int i = 0; i < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    enchantments[i].type = Magic::INVALID_ENCHANTMENT_TYPE_INDEX;
    enchantments[i].active_event_id = Event::INVALID_EVENT_ID;
  };
}


}
}
}

