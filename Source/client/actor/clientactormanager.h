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
#ifndef __CLIENT_ACTOR_CLIENTACTORMANAGER_H__
#define __CLIENT_ACTOR_CLIENTACTORMANAGER_H__
#pragma once


#include "shared/evidyon-limits.h"
#include "clientactorinstance.h"
#include <dcx/memorypool>
#include <set>
#include <hash_map>
#include "shared/actor/actorsyncflags.h"
#include "shared/actor/actoralignment.h"
#include "shared/binding/bindingptr.h"
#include "shared/actor/actorinteraction.h"
#include "shared/magic/enchantmenttype.h"
#include "shared/magic/enchantmenttypeindex.h"
#include "shared/event/eventindex.h"

namespace Evidyon {
namespace Client {
class ClientData;
}
namespace Actor {
class ActorRenderer;
struct ActorSlowSync;
struct ActorTemplate;
struct ActorDescription;
}
namespace Event {
class EventManager;
}
}

namespace Evidyon {
namespace Actor {
namespace Client {




//----[  ClientActorManager  ]-------------------------------------------------
class ClientActorManager {
  static const int MAX_ACTOR_INSTANCES = Limits::ACTORS_ON_SCREEN;
  typedef dcx::dcxMemoryPool<ClientActorInstance,
                             MAX_ACTOR_INSTANCES> Actors;
  typedef stdext::hash_map<ActorID,ClientActorInstance*> ActorMap;
  typedef stdext::hash_map<ActorID,std::string> ActorNameMap;
  struct Enchantment {
    const Evidyon::Event::EventIndex* event_indices;
    size_t number_of_type;
  };
public:
  typedef std::set<ActorID> ActorIDSet;

  // Callback class for adding spoken phrases to a chat log
  class ActorSpeaksFunctionoid {
  public:
    virtual ~ActorSpeaksFunctionoid();
    virtual void say(const char* name,
                     Actor::ActorAlignment alignment,
                     const std::string& text) = 0;
  };

  // Callback class for rendering speech text
  class ActorDrawSpeechTextFunctionoid {
  public:
    virtual ~ActorDrawSpeechTextFunctionoid();
    virtual void draw(float actor_x,
                      float actor_z,
                      float actor_height,
                      Actor::ActorAlignment alignment,
                      const std::string& last_line,
                      float last_line_fade,
                      const std::string& current_line,
                      float current_line_fade,
                      float line_offset) = 0;
  };

  // Callback class for rendering actor names
  class ActorDrawNameTextFunctionoid {
  public:
    virtual ~ActorDrawNameTextFunctionoid();
    virtual void draw(float actor_x,
                      float actor_z,
                      Actor::ActorAlignment alignment,
                      const char* name,
                      const char* guild) = 0;
  };


public:
  ClientActorManager();
  ~ClientActorManager();
  bool create(ActorRenderer* renderer, Event::EventManager* event_manager);
  void destroy();

  // Call before updating clients with slowSync, and save the counter value
  // temporarily.
  void beginSlowSync(unsigned int* counter_value);

  // Syncs the state of the given actor with data from the server.
  void slowSync(double time,
                double time_since_last_sync,
                const ActorSlowSync* sync);

  // Call this after updating clients with slowSync so that those not updated
  // will be erased.
  void endSlowSync(unsigned int counter_value);

  // Syncs the state of the client's actor.  This is done more frequently and
  // in a different way from slowSync.
  void fastSync(double time,
                double time_since_last_sync,
                float x,
                float y,
                ActorSyncFlags flags);

  // Adds data about actor templates to the manager.
  bool initNumberOfActorTemplates(size_t count);
  bool initActorTemplate(size_t index, const ActorTemplate* actor_template);
  void clearActorTemplates();

  // Sets up the enchantment types.
  // TODO: copy the parameters internally.  Right now I know that in
  // EvidyonClient::acquireResources, indices are allocated with new[] and are just
  // passed in here.
  void initEnchantmentType(Magic::EnchantmentType enchantment,
                           const Event::EventIndex* indices,
                           size_t number_of_type);

  // Obtains the template at the given index.  If the template index is invalid,
  // this method returns NULL.
  const ActorTemplate* actorTemplate(ActorTemplateIndex actor_template);

  // Allocates a new client actor.  If an actor for the client already exists,
  // it is erased (but this causes an assert() in debug mode, since that
  // shouldn't happen).
  ClientActorInstance* acquireClientActor(
    const char* name,
    const ActorDescription* description);

  // Obtains the current client actor.  If the client's actor doesn't exist,
  // this method raises an assert and returns null.
  ClientActorInstance* getClientActor();

  // Searches for the actor in the global table.  This method returns NULL
  // if the actor doesn't exist.
  ClientActorInstance* getActor(ActorID actor_id);
  const ClientActorInstance* getActor(ActorID actor_id) const;

  // Removes the actor with the given ID from the manager
  void erase(ActorID actor_id);

  // Updates the name of the actor with the given ID number.  If the actor
  // doesn't exist, it is acquired.
  void setActorName(ActorID actor_id, const char* name);
  void setActorGuildMemberName(ActorID actor_id,
                               const char* name,
                               const char* guild_name);
  void setActorGuildLeaderName(ActorID actor_id,
                               const char* name,
                               const char* guild_name);
  void setActorGuildFounderName(ActorID actor_id,
                                const char* name,
                                const char* guild_name);

  // Changes the description of the actor specified by in the parameter.  The
  // actor is acquired if it doesn't exist.
  void setActorDescription(double time, const ActorDescription* description);

  // Changes what an actor is saying.  If the target actor id is valid, this
  // method replaces all % symbols in 'speech' with the target's name.
  void setActorSpeech(ActorID speaking_actor_id,
                      ActorID target_actor_id,
                      const char* speech);

  // Erases all actors, unless the flag is set in which case the client's actor
  // is preserved.  This is useful for changing maps.
  void clear(bool erase_client_actor);

  // Updates the state of all of the actors.  If an actor changes its phrase,
  // the functionoid's 'say' method is invoked so that the speech can be added
  // to a chat log.
  void update(double time,
              double time_since_last_update,
              Evidyon::Client::ClientData* client_data,
              ActorSpeaksFunctionoid* actor_speaks_functionoid);

  // Invokes the speech functionoid's draw method for each actor that is
  // currently saying something.
  void drawSpeech(ActorDrawSpeechTextFunctionoid* functionoid);

  // Invokes the name text functionoid's draw method for each visible actor
  void drawNames(ActorDrawNameTextFunctionoid* functionoid);

  // Finds the actor with the shortest distance to the given location.  Returns
  // the square of the distance to the returned actor, or a very large value
  // if no actor was found.
  float getActorClosestToLocation(bool find_client_actor,
                                  bool find_dead_actors,
                                  float x,
                                  float z,
                                  ActorID* actor_id);
  float getActorClosestToLocation(bool find_client_actor,
                                  bool find_dead_actors,
                                  float x,
                                  float z,
                                  ClientActorInstance** instance);

  // Returns the interaction that should occur when the client clicks on the
  // actor with the given ID number.  This method takes into account how
  // close the client is to the target.
  ActorInteraction getActorInteraction(ActorID actor) const;


public:
  
  // These methods are used to create a network message sent to the server
  // that obtains the full description of those actors missing one.  Once
  // all the descriptions have been processed, calling
  // actorDescriptionsRequested clears the 'needing' list.
  ActorIDSet::const_iterator actorsNeedingDescriptionBegin() const;
  ActorIDSet::const_iterator actorsNeedingDescriptionEnd() const;
  void actorDescriptionsRequested();

  // These three methods are used in the same way as the three actor
  // description methods are
  ActorIDSet::const_iterator actorsNeedingNameBegin() const;
  ActorIDSet::const_iterator actorsNeedingNameEnd() const;
  void actorNamesRequested();

private:
  void zero();

  // Obtains a brand new actor instance and inserts it in the actor map.
  ClientActorInstance* acquire(ActorID actor_id);

  // Frees the memory used by the given actor instance
  void release(ClientActorInstance* instance);

  // Acquires or looks up the actor with the given ID.  If the newly_created
  // parameter is valid, it is set to specify which operation was done.
  ClientActorInstance* obtainActor(ActorID actor_id,
                                   bool is_dead,
                                   bool* newly_created);

  // Obtains the name of the given actor from the internal name map.  If 
  // it is found, it is removed.
  void retrieveName(ActorID actor_id,
                    char* buffer,
                    size_t buffer_length);

  // Gets the type indices from the 'enchantments' field of an actor
  // description
  void unpackEnchantments(
    unsigned short packed_enchantments,
    Magic::EnchantmentTypeIndex enchantments[Magic::NUMBER_OF_ENCHANTMENT_TYPES]);

  void setInstanceEnchantment(double time,
                              ClientActorInstance* instance,
                              unsigned short enchantment_mask);

  void releaseInstanceEnchantments(ClientActorInstance* instance);

private:

  // All templates for actor types
  ActorTemplate* templates_;
  size_t number_of_templates_;

  // Rendering objects
  ActorRenderer* renderer_;

  // The actor instance for the client
  ClientActorInstance* client_actor_;

  // Actor memory source
  Actors actors_;

  // All of the actors in the game are stored in this lookup table
  ActorMap actor_map_;

  // Actors are based on templates.  These templates are sometimes missing
  // important information, such as an actor's full description or the actor's
  // name.  These sets are used to generate server requests for this info.
  ActorIDSet actors_needing_names_;
  ActorIDSet actors_requested_names_;
  ActorIDSet actors_needing_description_;
  ActorIDSet actors_requested_description_;

  // Requested actor names are stored here if setActorName can't immediately
  // store the name of an actor in its targeted instance.
  ActorNameMap actor_names_;

  // This value is used when slow-syncing actors to erase those that are no
  // longer being updated by the server (this happens when they go out of
  // range or disappear from view).
  unsigned int slow_sync_counter_;

  // These are the descriptions of which events are created on actors for
  // an enchantment of the given type.
  Enchantment enchantments_[Magic::NUMBER_OF_ENCHANTMENT_TYPES];

  // Used to create enchantments
  Event::EventManager* event_manager_;
};

}
}
}

#endif