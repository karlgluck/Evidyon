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
#include "clientactormanager.h"
#include "shared/actor/actorslowsync.h"
#include "shared/actor/actortemplate.h"
#include "shared/actor/actordescription.h"
#include "shared/actor/actorrenderer.h"
#include "shared/actor/renderableactorinstance.h"
#include "common/safemacros.h"
#include "common/null.h"
#include <assert.h>
#include "shared/event/eventmanager.h"


#include <dc/debug> // TEMPORARY

namespace Evidyon {
namespace Actor {
namespace Client {




//----[  ~ActorSpeaksFunctionoid  ]--------------------------------------------
ClientActorManager::ActorSpeaksFunctionoid::~ActorSpeaksFunctionoid() {}


//----[  ~ActorDrawSpeechTextFunctionoid  ]------------------------------------
ClientActorManager::ActorDrawSpeechTextFunctionoid::~ActorDrawSpeechTextFunctionoid() {}




//----[  ~ActorDrawNameTextFunctionoid  ]--------------------------------------
ClientActorManager::ActorDrawNameTextFunctionoid::~ActorDrawNameTextFunctionoid() {}
  

//----[  ClientActorManager  ]-------------------------------------------------
ClientActorManager::ClientActorManager() {
  zero();
}


//----[  ClientActorManager  ]-------------------------------------------------
ClientActorManager::~ClientActorManager() {
  destroy();
}


//----[  create  ]-------------------------------------------------------------
bool ClientActorManager::create(ActorRenderer* renderer,
                                Event::EventManager* event_manager) {
  if (!actors_.create()) return false;
  renderer_ = renderer;
  event_manager_ = event_manager;
  return true;
}




//----[  destroy  ]------------------------------------------------------------
void ClientActorManager::destroy() {
  for (int i = 0; i < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    SAFE_DELETE_ARRAY(enchantments_[i].event_indices);
  }
  clear(true);
  actors_.destroy();
  clearActorTemplates();
  zero();
}





//----[  beginSlowSync  ]------------------------------------------------------
void ClientActorManager::beginSlowSync(unsigned int* counter_value) {
  *counter_value = ++slow_sync_counter_;
}



//----[  slowSync  ]-----------------------------------------------------------
void ClientActorManager::slowSync(double time,
                                  double time_since_last_sync,
                                  const ActorSlowSync* sync) {
  bool newly_created;
  ClientActorInstance* instance
    = obtainActor(sync->id,
                  sync->data.flags.state == ActorSyncFlags::MOVEMENT_DEAD,
                  &newly_created);
  if (!instance) return;
  if (newly_created) {
    renderer_->firstSync(instance->renderable_instance,
                         sync->data.unpackX(),
                         sync->data.unpackY(),
                         sync->data.flags);
  } else {
    renderer_->sync(instance->renderable_instance,
                    time_since_last_sync,
                    time,
                    sync->data.unpackX(),
                    sync->data.unpackY(),
                    sync->data.flags);
  }
  instance->slow_sync_counter = slow_sync_counter_;
}



//----[  endSlowSync  ]--------------------------------------------------------
void ClientActorManager::endSlowSync(unsigned int counter_value) {
  for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
    ClientActorInstance* instance = i.get();
    assert(instance);
    // sub 1 so that a doubled-up slow sync (for > 60 actors) doesn't start
    // erasing things from the world
    // DONT add 1: the slow sync counter can wrap!
    if (instance->slow_sync_counter < slow_sync_counter_ - 1) {
      // The client actor should never fall in here, because its counter is
      // set to UINT_MAX and it is never slow-synced.
      assert(instance != client_actor_);
      release(instance);
    }
  }
}



//----[  fastSync  ]-----------------------------------------------------------
void ClientActorManager::fastSync(double time,
                                  double time_since_last_sync,
                                  float x,
                                  float y,
                                  ActorSyncFlags flags) {
  ClientActorInstance* instance = getClientActor();
  assert(instance);
  if (!instance) return;
  renderer_->sync(instance->renderable_instance,
                  time_since_last_sync,
                  time,
                  x,
                  y,
                  flags);
}



//----[  initNumberOfActorTemplates  ]-----------------------------------------
bool ClientActorManager::initNumberOfActorTemplates(size_t count) {
  SAFE_DELETE_ARRAY(templates_);
  templates_ = new ActorTemplate[count];
  assert(templates_);
  if (!templates_) { number_of_templates_ = 0; return false; }
  number_of_templates_ = count;
  return true;
}




//----[  initActorTemplate  ]--------------------------------------------------
bool ClientActorManager::initActorTemplate(
    size_t index,
    const ActorTemplate* actor_template) {
  assert(index < number_of_templates_);
  if (index >= number_of_templates_) return false;
  memcpy(&templates_[index], actor_template, sizeof(ActorTemplate));
  return true;
}




//----[  clearActorTemplates  ]------------------------------------------------
void ClientActorManager::clearActorTemplates() {
  SAFE_DELETE_ARRAY(templates_);
  number_of_templates_ = 0;
}



//----[  initEnchantmentType  ]------------------------------------------------
void ClientActorManager::initEnchantmentType(Magic::EnchantmentType enchantment,
                                             const Event::EventIndex* indices,
                                             size_t number_of_type) {
  enchantments_[enchantment].event_indices = indices;
  enchantments_[enchantment].number_of_type = number_of_type;
}



//----[  actorTemplate  ]------------------------------------------------------
const ActorTemplate* ClientActorManager::actorTemplate(
    ActorTemplateIndex actor_template) {
  assert(actor_template < number_of_templates_);
  if (actor_template >= number_of_templates_) return NULL;
  return &templates_[actor_template];
}




//----[  acquireClientActor  ]-------------------------------------------------
ClientActorInstance* ClientActorManager::acquireClientActor(
    const char* name,
    const ActorDescription* description) {
  assert(client_actor_ == NULL);
  setActorDescription(0.0, description); // todo: real time
  setActorName(description->id, name);
  ClientActorInstance* instance = getActor(description->id);
  assert(instance);
  client_actor_ = instance;
  instance->slow_sync_counter = -1; // assign the max counter value
  return instance;
}




//----[  getClientActor  ]-----------------------------------------------------
ClientActorInstance* ClientActorManager::getClientActor() {
  assert(client_actor_);
  return client_actor_;
}






//----[  getActor  ]-----------------------------------------------------------
ClientActorInstance* ClientActorManager::getActor(ActorID actor_id) {
  ActorMap::iterator i = actor_map_.find(actor_id);
  return i == actor_map_.end() ? NULL : i->second;
}


//----[  getActor  ]-----------------------------------------------------------
const ClientActorInstance* ClientActorManager::getActor(ActorID actor_id) const {
  ActorMap::const_iterator i = actor_map_.find(actor_id);
  return i == actor_map_.end() ? NULL : i->second;
}



//----[  erase  ]--------------------------------------------------------------
void ClientActorManager::erase(ActorID actor_id) {
  ClientActorInstance* instance = getActor(actor_id);
  if (instance) release(instance);
}



//----[  setActorName  ]-------------------------------------------------------
void ClientActorManager::setActorName(ActorID actor_id, const char* name) {
  ClientActorInstance* instance = getActor(actor_id);
  if (instance) {
    strcpy_s(instance->renderable_instance->base.name,
             ActorTemplate::MAX_NAME_LENGTH,
             name);
    instance->guild_name[0] = '\0';
  } else {
    // save to match with a description later
    actor_names_.insert(ActorNameMap::value_type(actor_id, name));
  }
  actors_needing_names_.erase(actor_id);
  actors_requested_names_.erase(actor_id);
}




//----[  setActorGuildMemberName  ]--------------------------------------------
void ClientActorManager::setActorGuildMemberName(ActorID actor_id,
                                                 const char* name,
                                                 const char* guild_name) {
  ClientActorInstance* instance = getActor(actor_id);
  if (instance) {
    strcpy_s(instance->renderable_instance->base.name,
             ActorTemplate::MAX_NAME_LENGTH,
             name);
    sprintf_s(instance->guild_name,
      Guild::MAX_GUILD_MEMBER_NAME_TAG_LENGTH,
      "[%s]",
      guild_name);
  }
}




//----[  setActorGuildLeaderName  ]--------------------------------------------
void ClientActorManager::setActorGuildLeaderName(ActorID actor_id,
                                                 const char* name,
                                                 const char* guild_name) {
  ClientActorInstance* instance = getActor(actor_id);
  if (instance) {
    strcpy_s(instance->renderable_instance->base.name,
             ActorTemplate::MAX_NAME_LENGTH,
             name);
    sprintf_s(instance->guild_name,
      Guild::MAX_GUILD_MEMBER_NAME_TAG_LENGTH,
      "[Leader of %s]",
      guild_name);
  }
}




//----[  setActorGuildFounderName  ]-------------------------------------------
void ClientActorManager::setActorGuildFounderName(ActorID actor_id,
                                                  const char* name,
                                                  const char* guild_name) {
  ClientActorInstance* instance = getActor(actor_id);
  if (instance) {
    strcpy_s(instance->renderable_instance->base.name,
             ActorTemplate::MAX_NAME_LENGTH,
             name);
    sprintf_s(instance->guild_name,
      Guild::MAX_GUILD_MEMBER_NAME_TAG_LENGTH,
      "[Founder of %s]",
      guild_name);
  }
}






//----[  setActorDescription  ]------------------------------------------------
void ClientActorManager::setActorDescription(
    double time,
    const ActorDescription* description) {
  assert(renderer_);
  ActorID actor_id = description->id;
  ClientActorInstance* instance = getActor(actor_id);
  RenderableActorInstance* renderable_instance = NULL;
  bool create = !instance;
  if (create) {
    instance = acquire(actor_id);
    if (!instance) return;  // instance couldn't be acquired

    // Unpack the ID number to retrieve the actor's template type
    ActorTemplateIndex actor_template_index;
    UnpackActorID(actor_id, &actor_template_index);
    const ActorTemplate* actor_template
      = actorTemplate(actor_template_index);

    // Create the actor rendering instance based on the template
    bool is_dead =
      description->sync.flags.state == ActorSyncFlags::MOVEMENT_DEAD;
    renderable_instance
      = renderer_->acquire(
          actor_template,
          ActorProfileIndex(description->profile_index),
          SkinnedMesh::SkinnedMeshFormIndex(description->form),
          is_dead);
    if (!renderable_instance) {
      release(instance);
      return;
    }

    // If there is a name waiting for this instance, copy it
    retrieveName(actor_id,
                 renderable_instance->base.name,
                 ActorTemplate::MAX_NAME_LENGTH);

    // This must be done before the first sync so that any action
    // animations and other profile-based data are set correctly.
    renderable_instance->base.profile_specialization
      = ActorProfileSpecializationType(
          description->equipment_based_profile_specialization);
    renderable_instance->profile_specialization
      = renderable_instance->base.profile_specialization;

    renderer_->firstSync(renderable_instance,
                         description->sync.unpackX(),
                         description->sync.unpackY(),
                         description->sync.flags);

    instance->renderable_instance = renderable_instance;

    // We now have this actor's description
    actors_requested_description_.erase(actor_id);
    actors_needing_description_.erase(actor_id);

  } else {

    // actor already exists, so it needs to be changed into its new form
    renderable_instance = instance->renderable_instance;
    renderable_instance->base.profile_specialization
      = ActorProfileSpecializationType(
          description->equipment_based_profile_specialization);
    renderable_instance->profile_specialization =// TODO: get rid of copied specialization, it doesn't do anything
      renderable_instance->base.profile_specialization;

    renderer_->mutate(
      renderable_instance,
      ActorProfileIndex(description->profile_index),
      SkinnedMesh::SkinnedMeshFormIndex(description->form));
  }

  { // update the equipment that the actor is wearing
    Scenery::SceneryIndex equipment_scenery[NUMBER_OF_ACTOR_EQUIPMENT_SLOTS];
    for (int i = 0; i < NUMBER_OF_ACTOR_EQUIPMENT_SLOTS; ++i) {
      unsigned __int16 scenery = description->equipment_scenery[i];
      equipment_scenery[i]
        = (scenery == unsigned __int16(-1))
          ? Evidyon::Scenery::INVALID_SCENERY_INDEX
          : Scenery::SceneryIndex(scenery);
    }

    // Update all of the attached scenery.  If this instance was just created,
    // all of the scenery needs to be attached.
    renderer_->updateEquipment(renderable_instance,
                               equipment_scenery,
                               create); // reequip all based on create flag
  }

  { // update the enchantments
    setInstanceEnchantment(time, instance, description->enchantments);
  }

  // set misc. flags
  renderable_instance->base.alignment =
    ActorAlignment(description->alignment);
}




//----[  setActorSpeech  ]-----------------------------------------------------
void ClientActorManager::setActorSpeech(ActorID speaking_actor_id,
                                        ActorID target_actor_id,
                                        const char* speech) {
  ClientActorInstance* speaking_actor = getActor(speaking_actor_id);
  if (!speaking_actor) return;
  ClientActorInstance* target_actor = getActor(target_actor_id);
  std::string speech_str = speech;
  if (target_actor) {
    const std::string percent("%"); // replace all % symbols with target names
    const std::string replacement
      = target_actor->renderable_instance->base.name;
    size_t offset = 0;
    for (offset  = speech_str.find(percent, offset);
         offset != speech_str.npos;
         offset  = speech_str.find(percent, offset)) {
      speech_str.replace(offset, 1, replacement);
      offset += replacement.length();
    }
  }
  speaking_actor->setSpeech(target_actor == client_actor_,
                            speech_str);
}



//----[  clear  ]--------------------------------------------------------------
void ClientActorManager::clear(bool erase_client_actor) {
  actor_map_.clear();
  if (erase_client_actor) {

    // free all renderable instances
    if (renderer_) renderer_->clearInstances();

    // get rid of all of the events
    for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
      ClientActorInstance* instance = i.get();
      if (instance == client_actor_) continue;
      releaseInstanceEnchantments(instance);
    }

    // clear everything
    actors_.releaseAll();
    client_actor_ = NULL;

  } else {

    // remove all but client
    for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
      ClientActorInstance* instance = i.get();
      if (instance == client_actor_) continue;
      renderer_->release(instance->renderable_instance);
      releaseInstanceEnchantments(instance);
      i.remove();
    }

    // re-add the client to the map
    actor_map_.insert(ActorMap::value_type(client_actor_->id, client_actor_));
  }

  // client shouldn't ever be in these lists, so just clear them
  actors_needing_names_.clear();
  actors_requested_names_.clear();
  actors_needing_description_.clear();
  actors_requested_description_.clear();
  actor_names_.clear();
}





//----[  update  ]-------------------------------------------------------------
void ClientActorManager::update(
    double time,
    double time_since_last_update,
    Evidyon::Client::ClientData* client_data,
    ActorSpeaksFunctionoid* actor_speaks_functionoid) {
  for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
    ClientActorInstance* instance = i.get();
    assert(instance);
    renderer_->updateInstance(
      instance->renderable_instance,
      time,
      time_since_last_update,
      client_data);
    if (instance->setNextSpokenPhrase(time)) {
      actor_speaks_functionoid->say(
        instance->renderable_instance->base.name,
        instance->renderable_instance->base.alignment,
        instance->speech.displayed_text[instance->speech.current_line]);
    }
  }
}




//----[  drawSpeech  ]---------------------------------------------------------
void ClientActorManager::drawSpeech(
    ActorDrawSpeechTextFunctionoid* functionoid) {
  assert(functionoid);
  for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
    ClientActorInstance* instance = i.get();
    assert(instance);
    if (instance->speech.displayed_text[instance->speech.current_line].empty()) {
      continue;
    }
    functionoid->draw(
      instance->renderable_instance->current_movement_state.x,
      instance->renderable_instance->current_movement_state.y,
      1.65f, // TODO: configurable height
      instance->renderable_instance->base.alignment,
      instance->speech.displayed_text[instance->speech.last_line],
      instance->speech.displayed_text_fade[instance->speech.last_line],
      instance->speech.displayed_text[instance->speech.current_line],
      instance->speech.displayed_text_fade[instance->speech.current_line],
      instance->speech.displayed_text_top_line_offset);
  }
}



//----[  drawNames  ]----------------------------------------------------------
void ClientActorManager::drawNames(ActorDrawNameTextFunctionoid* functionoid) {
  assert(functionoid);
  for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
    ClientActorInstance* instance = i.get();
    assert(instance);
    const Actor::RenderableActorInstance* renderable_instance
      = instance->renderable_instance;
    functionoid->draw(
      renderable_instance->current_movement_state.x,
      renderable_instance->current_movement_state.y,
      renderable_instance->base.alignment,
      renderable_instance->base.name,
      instance->guild_name);
  }
}


//----[  getActorClosestToLocation  ]------------------------------------------
float ClientActorManager::getActorClosestToLocation(bool find_client_actor,
                                                    bool find_dead_actors,
                                                    float x,
                                                    float z,
                                                    ActorID* actor_id) {
  float closest_distance_sq = 3.402823466e+15F;
  *actor_id = Actor::INVALID_ACTOR_ID;
  for (Actors::Iterator i(&actors_); i.hasNext(); i.advance()) {
    ClientActorInstance* instance = i.get();
    assert(instance);
    if ((!find_client_actor && (instance == client_actor_)) || 
        (!find_dead_actors && (instance
                                ->renderable_instance
                                  ->base_animation_state
                               == Actor::RenderableActorInstance::DEAD))) {
      continue;
    }
    float actor_x, actor_z;
    instance->getPosition(&actor_x, &actor_z);
    actor_x -= x;
    actor_z -= z;
    float distance_sq = actor_x * actor_x + actor_z * actor_z;
    if (distance_sq < closest_distance_sq) {
      *actor_id = instance->id;
      closest_distance_sq = distance_sq;
    }
  }
  return closest_distance_sq;
}


//----[  getActorInteraction  ]------------------------------------------------
ActorInteraction ClientActorManager::getActorInteraction(ActorID actor) const {

  const ClientActorInstance* instance = getActor(actor);
  if (!instance) return INVALID_ACTOR_INTERACTION;
  float x, z, client_x, client_z;
  instance->getPosition(&x, &z);
  instance->getPosition(&client_x, &client_z);
  x -= client_x;
  z -= client_z;
  bool nearby = (x*x + z*z) < 2.0f * 2.0f;
  return nearby ? instance->renderable_instance->base.interaction : INVALID_ACTOR_INTERACTION;
}




//----[  actorsNeedingDescriptionBegin  ]--------------------------------------
ClientActorManager::ActorIDSet::const_iterator
ClientActorManager::actorsNeedingDescriptionBegin() const {
  return actors_needing_description_.begin();
}


//----[  actorsNeedingDescriptionEnd  ]----------------------------------------
ClientActorManager::ActorIDSet::const_iterator
ClientActorManager::actorsNeedingDescriptionEnd() const {
  return actors_needing_description_.end();
}


//----[  actorDescriptionsRequested  ]-----------------------------------------
void ClientActorManager::actorDescriptionsRequested() {
  actors_requested_description_.insert(
    actors_needing_description_.begin(),
    actors_needing_description_.end());
  actors_needing_description_.clear();
}




//----[  actorsNeedingNameBegin  ]---------------------------------------------
ClientActorManager::ActorIDSet::const_iterator
ClientActorManager::actorsNeedingNameBegin() const {
  return actors_needing_names_.begin();
}


//----[  actorsNeedingNameEnd  ]-----------------------------------------------
ClientActorManager::ActorIDSet::const_iterator
ClientActorManager::actorsNeedingNameEnd() const {
  return actors_needing_names_.end();
}


//----[  actorNamesRequested  ]------------------------------------------------
void ClientActorManager::actorNamesRequested() {
  actors_requested_names_.insert(
    actors_needing_names_.begin(),
    actors_needing_names_.end());
  actors_requested_names_.clear();
}


//----[  zero  ]---------------------------------------------------------------
void ClientActorManager::zero() {
  templates_ = NULL;
  number_of_templates_ = 0;
  renderer_ = NULL;
  client_actor_ = NULL;
  slow_sync_counter_ = 1;
  memset(enchantments_, 0, sizeof(enchantments_));
}




//----[  acquire  ]------------------------------------------------------------
ClientActorInstance* ClientActorManager::acquire(ActorID actor_id) {
  ClientActorInstance* actor = actors_.acquire();
  if (!actor) return NULL; // out of memory
  actor->zero();
  actor->id = actor_id;
  actor_map_.insert(ActorMap::value_type(actor_id, actor));
  return actor;
}





//----[  release  ]------------------------------------------------------------
void ClientActorManager::release(ClientActorInstance* instance) {
  assert(instance);
  releaseInstanceEnchantments(instance);
  if (instance->renderable_instance) {
    assert(renderer_);
    renderer_->release(instance->renderable_instance);
    instance->renderable_instance = NULL;
  }
  ActorID actor_id = instance->id;
  actors_needing_names_.erase(actor_id);
  actors_requested_names_.erase(actor_id);
  actors_needing_description_.erase(actor_id);
  actors_requested_description_.erase(actor_id);
  actor_names_.erase(actor_id);
  actor_map_.erase(instance->id);
  actors_.release(instance);
}



//----[  obtainActor  ]--------------------------------------------------------
ClientActorInstance* ClientActorManager::obtainActor(ActorID actor_id,
                                                     bool is_dead,
                                                     bool* newly_created) {
  ClientActorInstance* instance = getActor(actor_id);
  bool create = !instance;
  if (newly_created) *newly_created = create;
  if (!create) return instance; // instance already exists

  // If this actor is waiting for a description, it should already have an
  // actor created; therefore, it should never be in the 'needing description'
  // list or the 'needing names' list.
  assert(actors_needing_description_.end() ==
         actors_needing_description_.find(actor_id));
  assert(actors_needing_names_.end() ==
         actors_needing_names_.find(actor_id));
  //if (actors_needing_description_.end() !=
  //    actors_needing_description_.find(actor_id)) {
  //  return NULL;
  //}

  // Unpack the ID number to retrieve the actor's template type
  ActorTemplateIndex actor_template_index;
  UnpackActorID(actor_id, &actor_template_index);
  const ActorTemplate* actor_template = actorTemplate(actor_template_index);

  // Inspect the actor types
  switch (actor_template->template_type) {

    // Dynamic types.  Only the avatars need a name, though.  Names
    // are separated since the request for one requires so many
    // bytes to be sent back from the server.
    case AVATAR_TEMPLATE:
      actors_needing_names_.insert(actor_id);
    case MONSTER_TEMPLATE:
      actors_needing_description_.insert(actor_id);

    // It is likely these types will not show up correctly when first created
    // and that they will suddenly "jump" to their correcy form when the full
    // description is received.  However, it seems like it would be better in
    // such a fast-paced game to just go ahead and show them anyway.
      //return NULL;    // uncomment this option to hide until correct
      break;          // use this option to show immediately

    // Static types.  Shouldn't ever need their descriptions updated,
    // so we can create them just by knowing their ID.  Cool, eh?
    case STATIC_TEMPLATE:
    //case STATIC_MONSTER_TEMPLATE:
    //case GENERIC_NPC_TEMPLATE:
    //case STORAGE_NPC_TEMPLATE:
    //case BAZAAR_NPC_TEMPLATE:
    //case GEOSID_NPC_TEMPLATE:
    //case QUEST_NPC_TEMPLATE:
      break;

    default:
      // unknown template type--might need a description
      assert(false);
      break;
  }

  instance = acquire(actor_id);
  if (!instance) return false;  // instance couldn't be acquired

  // Create the actor rendering instance based on the template
  RenderableActorInstance* renderable_instance
    = renderer_->acquire(actor_template,
                         actor_template->actor_profile,
                         actor_template->skinned_mesh_form,
                         is_dead);
  if (!renderable_instance) {
    release(instance);
    return false;
  }

  // Attach all of this actor's default items.
  renderer_->updateEquipment(renderable_instance,
                             NULL,
                             false);
  // todo: real time
  //setInstanceEnchantment(0.0, instance, actor_template->enchantments);

  instance->renderable_instance = renderable_instance;
  return instance;
}






//----[  retrieveName  ]-------------------------------------------------------
void ClientActorManager::retrieveName(ActorID actor_id,
                                      char* buffer,
                                      size_t buffer_length) {
  ActorNameMap::iterator i = actor_names_.find(actor_id);
  if (i == actor_names_.end()) {
    if (actors_requested_names_.find(actor_id) ==
        actors_requested_names_.end()) {
      actors_needing_names_.insert(actor_id);
    }
  } else {
    strcpy_s(buffer, buffer_length, i->second.c_str());
    actor_names_.erase(i);
  }
}
//
//unsigned long MultPack(unsigned long p, unsigned long v, unsigned long o) {
//  return (p * o + v);
//}

unsigned long MultUnpack(unsigned long p, unsigned long o, unsigned long* v) {
  unsigned long lv = p % o;
  *v = lv;
  return p / o;
}

//----[  unpackEnchantments  ]-------------------------------------------------
void ClientActorManager::unpackEnchantments(
    unsigned short packed_enchantments,
    Magic::EnchantmentTypeIndex enchantments[Magic::NUMBER_OF_ENCHANTMENT_TYPES]) {
  unsigned long ul = packed_enchantments;
  for (int i = Magic::NUMBER_OF_ENCHANTMENT_TYPES; i > 0;) {
    // need to add 1 to 'o' (the "options" parameter) because 0 in an enchantment
    // type needs to be included.
    // ex. enchantments_in_type_[i] = 3 means there are 4 possible indices: 0, 1, 2, 3
    // MultPack requres that o > v, so 3 -> 4
    // see globalgamefilemanager.cpp (GlobalGamefileManager::packEnchantments)
    unsigned long v;
    ul = MultUnpack(ul, enchantments_[--i].number_of_type+1, &v);
    enchantments[i] = v;
  }
}




//----[  setInstanceEnchantment  ]---------------------------------------------
void ClientActorManager::setInstanceEnchantment(double time,
                                                ClientActorInstance* instance,
                                                unsigned short enchantment_mask) {
  Actor::RenderableActorInstance* renderable_instance
    = instance->renderable_instance;
  Magic::EnchantmentTypeIndex enchantments[Magic::NUMBER_OF_ENCHANTMENT_TYPES];
  unpackEnchantments(enchantment_mask, enchantments);
  for (int i = 0; i < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++i) {
    Event::EventID event_id
      = instance->enchantments[i].active_event_id;
    bool active = event_manager_->isActive(event_id);
    if ((!active && enchantments[i] != Evidyon::Magic::INVALID_ENCHANTMENT_TYPE_INDEX) ||
        instance->enchantments[i].type != enchantments[i]) {
      event_manager_->terminate(time, event_id);
      if (enchantments[i] != Evidyon::Magic::INVALID_ENCHANTMENT_TYPE_INDEX) {
        instance->enchantments[i].active_event_id = Event::EventManager::generateClientEventID();
        event_manager_->generate(
          instance->enchantments[i].active_event_id,
          enchantments_[i].event_indices[enchantments[i]-1],
          time,
          1.0f,
          renderable_instance->current_movement_state.x,
          renderable_instance->current_movement_state.y,
          instance->getBinding(),
          renderable_instance->current_movement_state.x,
          renderable_instance->current_movement_state.y,
          NULL,
          0,
          instance == client_actor_);
      } else {
        instance->enchantments[i].active_event_id = Event::INVALID_EVENT_ID;
      }
      instance->enchantments[i].type = enchantments[i];
    }
  }
}


//----[  releaseInstanceEnchantments  ]----------------------------------------
void ClientActorManager::releaseInstanceEnchantments(ClientActorInstance* instance) {
  for (int j = 0; j < Magic::NUMBER_OF_ENCHANTMENT_TYPES; ++j) {
    Event::EventID event_id = instance->enchantments[j].active_event_id;
    if (event_id != Event::INVALID_EVENT_ID) {
      event_manager_->terminate(0.0, event_id);
    }
  }
}

}
}
}

