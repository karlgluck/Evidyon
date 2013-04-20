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
#ifndef __SERVER_ACTORCONTROLLER_H__
#define __SERVER_ACTORCONTROLLER_H__
#pragma once


#include "shared/actor/actordescription.h"
#include "shared/actor/actorinteraction.h"
#include "common/number.h"

namespace Evidyon {

struct ActorStateDescriptionData;
class ActorInstance;
class Client;
class NormalMeleeMonsterLifeformAIController;
class InteractiveNPCLifeformAIController;



//----[  ActorController  ]----------------------------------------------------
class ActorController {
public:

  // This method only returns a non-NULL pointer if the controller is a client.  The
  // default implementation returns 'NULL'
  virtual Client* getClient() const;
  virtual NormalMeleeMonsterLifeformAIController* getNormalMeleeMonsterLifeformAIController() const;
  virtual InteractiveNPCLifeformAIController* getInteractiveNPCLifeformAIController() const;

  inline bool isClient() const { return 0 != getClient(); }
  inline bool isMonster() const { return 0 != getNormalMeleeMonsterLifeformAIController(); }

  virtual void fillDescription(Evidyon::Actor::ActorDescription* description);

  // Send a packet to the client telling this actor's name.  This must be virtual
  // so that guild-names can be handled.
  virtual void tellName(Client* client);

  // Called when the client is triggering an interaction with this actor.
  // Returns 'true' if the interaction is allowable.  Default implementation
  // returns false for everything except ATTACK.
  virtual bool onClientInteraction(Client* client,
                                   ActorInstance* client_actor,
                                   Actor::ActorInteraction interaction);

  // Modifys the actor's HP in some way.  'magical' is set for spells;
  // If the controller has armor, changeHP should apply the armor's effect
  // accordingly.
  virtual Number changeHP(ActorInstance* source,
                          bool magical,
                          Number amount) = 0;
};


}

#endif