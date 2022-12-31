//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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
#include "triggerinstance_geosidactivate.h"
#include "server/actor.h"
#include "server/globalworldmanager.h"
#include "server/geosid/globalgeosidmanager.h"
#include "server/geosid/geosidinstance.h"
#include "server/client.h"
#include "server/guild/guildinstance.h"

namespace Evidyon {
namespace Trigger {
namespace Server {



  
//----[  TriggerInstance_GeosidActivate  ]-------------------------------------
TriggerInstance_GeosidActivate::TriggerInstance_GeosidActivate(
    const Evidyon::Trigger::Trigger_GeosidActivate* data) {
  memcpy(&data_, data, sizeof(data_));
}



//----[  onEnter  ]------------------------------------------------------------
void TriggerInstance_GeosidActivate::onEnter(ActorInstance* actor) {
  Evidyon::Client* client = actor->getController()->getClient();
  if (!client) return;
  Number x, z;
  actor->getPosition(&x, &z);
  Evidyon::Server::GeosidInstance* geosid
    =  Evidyon::Server::GlobalGeosidManager::singleton()->getGeosidIfInRange(
        data_.geosid,
        actor->getMapIndex(),
        x,
        z);
  assert(geosid);
  if (!geosid) return;

  // first, try to go through the geosid's portal
  if (geosid->onEnterCheckPortal(client, actor)) return;

  // next, try to attack the geosid
  Evidyon::Server::GuildInstance* guild = client->getGuild();
  if (guild) {
    guild->attackGeosid(geosid, client);
  }
}



}
}
}
