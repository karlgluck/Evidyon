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
#ifndef __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_INTERACTIVENPC_H__
#define __SERVER_LIFEFORMAI_LIFEFORMAIINSTANCE_INTERACTIVENPC_H__
#pragma once


#include "lifeformaiinstance.h"
#include "shared/time/timer.h"
#include "server/actorpointer.h"
#include "server/actorcontroller.h"

namespace Evidyon {
namespace LifeformAI {
struct LifeformAI_InteractiveNPC;
}
}

namespace Evidyon {
namespace Server {



//----[  LifeformAIInstance_InteractiveNPC  ]----------------------------------
class LifeformAIInstance_InteractiveNPC : public LifeformAIInstance,
                                          public ActorController {
public:
  LifeformAIInstance_InteractiveNPC(
      Number x,
      Number z,
      const LifeformAI::LifeformAI_InteractiveNPC* description);

public: // implement LifeformAIInstance
  virtual bool acquire(Map* map, GlobalActorManager* actor_manager);
  virtual bool update(double time, double time_since_last_update);
  virtual bool shouldReleaseWithWorldRegion(WorldRegion* region);
  virtual void release(GlobalActorManager* actor_manager);
  virtual void despawn();

public: // implement ActorController
  virtual Number changeHP(ActorInstance* source, bool magical, Number amount);
  virtual void fillDescription(Evidyon::Actor::ActorDescription* description);
  virtual bool onClientInteraction(Client* client,
                                   ActorInstance* client_actor,
                                   Actor::ActorInteraction interaction);

private:

  // Causes the actor to say one of this NPC's phrases
  void speakPhrase(Actor::ActorID target_actor);

private:
  const LifeformAI::LifeformAI_InteractiveNPC* description_;
  ActorPointer actor_;
  Number x_, z_;
};




}
}

#endif