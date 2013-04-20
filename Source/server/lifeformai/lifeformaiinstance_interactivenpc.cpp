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
#include "lifeformaiinstance_interactivenpc.h"
#include "shared/lifeformai/lifeformai_interactivenpc.h"
#include "server/globalactormanager.h"
#include "server/worldregion.h"
#include "server/actor.h"
#include "server/client.h"

#include <dc/debug>


namespace Evidyon {
namespace Server {

  
//----[  LifeformAIInstance_InteractiveNPC  ]------------------------------
LifeformAIInstance_InteractiveNPC::LifeformAIInstance_InteractiveNPC(
    Number x,
    Number z,
    const LifeformAI::LifeformAI_InteractiveNPC* description) {
  actor_.reset();
  description_ = description;
  x_ = x;
  z_ = z;
}




//----[  acquire  ]------------------------------------------------------------
bool LifeformAIInstance_InteractiveNPC::acquire(
    Map* map,
    GlobalActorManager* actor_manager) {
  actor_manager->acquireActor(&actor_,
                              this,
                              description_->actor_template_index,
                              map,
                              x_.toFloat(),
                              z_.toFloat());
  if (actor_.invalid()) return false;
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  my_actor->setAlive();
  my_actor->setLevel(0);
  my_actor->setMemberOfFaction(ActorInstance::NEUTRAL_NPC);
  my_actor->setSize(0.5);
  todo("karl", "set the size of an interactive npc");
  my_actor->enterWorld();
  return true;
}




//----[  update  ]-------------------------------------------------------------
bool LifeformAIInstance_InteractiveNPC::update(
    double time,
    double time_since_last_update) {
  if (actor_.invalid() || description_ == NULL) return false;
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();

  Number next_x = x_;
  Number next_z = z_;
  
  { // Advance the lifeform's state
    WorldRegion* regions_left[9];
    WorldRegion* regions_entered[9];
    int number_of_regions_left=0, number_of_regions_entered=0;
    if (my_actor->update(next_x,
                         next_z,
                         regions_left,
                        &number_of_regions_left,
                         regions_entered,
                        &number_of_regions_entered)) {
    }

    // Make the actor authoritative, so that our position is updated if we
    // get teleported or portaled somewhere.
    my_actor->getPosition(&x_, &z_);
  }

  // This lifeform is still active
  return true;
}




//----[  shouldReleaseWithWorldRegion  ]---------------------------------------
bool LifeformAIInstance_InteractiveNPC::shouldReleaseWithWorldRegion(
    WorldRegion* region) {
  return actor_.invalid() ||
    (actor_.dereferenceAssumingValid()->getRegion() == region);
}




//----[  release  ]------------------------------------------------------------
void LifeformAIInstance_InteractiveNPC::release(GlobalActorManager* actor_manager) {
  ActorInstance* my_actor = actor_.dereference();
  if (!my_actor) return;
  actor_manager->releaseActor(&actor_);
}




//----[  despawn  ]------------------------------------------------------------
void LifeformAIInstance_InteractiveNPC::despawn() {
  description_ = NULL; // triggers the main loop to return false
}



//----[  changeHP  ]-----------------------------------------------------------
Number LifeformAIInstance_InteractiveNPC::changeHP(ActorInstance* source, bool magical, Number amount) {
  if (description_->type == LifeformAI::LifeformAI_InteractiveNPC::DAMAGE_TARGET) {
    ActorInstance* my_actor = actor_.dereferenceAssumingValid();
    my_actor->setAction(Actor::ACTORACTION_SPIN_ATTACK, 500);
    if (!magical) my_actor->setBloodFlag();
    char speech[128];
    sprintf_s(speech, 128, "%i", -amount.wholePart());
    source->say(speech);
    speakPhrase(source->getIDNumber());
  }
  return Number(0);
}




//----[  fillDescription  ]----------------------------------------------------
void LifeformAIInstance_InteractiveNPC::fillDescription(
    Evidyon::Actor::ActorDescription* description) {

}





//----[  onClientInteraction  ]------------------------------------------------
bool LifeformAIInstance_InteractiveNPC::onClientInteraction(
    Client* client,
    ActorInstance* client_actor,
    Actor::ActorInteraction interaction) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  assert(client_actor);
  using namespace Evidyon::LifeformAI;
  switch (description_->type) {
  case LifeformAI_InteractiveNPC::STORAGE:
    return (interaction == Actor::ACTORINTERACTION_OPEN_STORAGE);
  case LifeformAI_InteractiveNPC::BAZAAR:
    return (interaction == Actor::ACTORINTERACTION_OPEN_BAZAAR);
  case LifeformAI_InteractiveNPC::SPOKESMAN:
    if (interaction == Actor::ACTORINTERACTION_TRIGGER) {
      speakPhrase(client_actor->getIDNumber());
      return true;
    }
    break;
  case LifeformAI_InteractiveNPC::DAMAGE_TARGET:
    return interaction == Actor::ACTORINTERACTION_ATTACK;
  case LifeformAI_InteractiveNPC::HEALER:
    if (interaction == Actor::ACTORINTERACTION_TRIGGER) {
      my_actor->setAction(Actor::ACTORACTION_CAST_AREA_SPELL, 1000);
      speakPhrase(client_actor->getIDNumber());
      client->changeHP(NULL, true, +9999);
      client->changeMP(+9999);
      return true;
    }
    break;
  default:
    assert(false); // some sort of unknown NPC type
  }

  // Can't process this type
  return false;
}



//----[  speakPhrase  ]--------------------------------------------------------
void LifeformAIInstance_InteractiveNPC::speakPhrase(Actor::ActorID target_actor) {
  ActorInstance* my_actor = actor_.dereferenceAssumingValid();
  my_actor->sayToActor(
    target_actor,
    description_->phrases[rand()%LifeformAI::LifeformAI_InteractiveNPC::NUMBER_OF_PHRASES]);
}



}
}

