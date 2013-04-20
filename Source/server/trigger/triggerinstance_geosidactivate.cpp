//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2008, 2009, 2010 Karl Gluck                                  //
//                                                                           //
//  Evidyon is free software: you can redistribute it and/or modify          //
//  it under the terms of the GNU General Public License as published by     //
//  the Free Software Foundation, either version 3 of the License, or        //
//  (at your option) any later version.                                      //
//                                                                           //
//  Evidyon is distributed in the hope that it will be useful,               //
//  but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//  GNU General Public License for more details.                             //
//                                                                           //
//  You should have received a copy of the GNU General Public License        //
//  along with Evidyon.  If not, see <http://www.gnu.org/licenses/>.         //
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
