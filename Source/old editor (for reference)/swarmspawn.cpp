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
#include "swarmspawn.h"
#include "../shared/server-editor/swarmspawndescription.h"



SwarmSpawn::SwarmSpawn() {
  member("Lifeform AI", &ai_lifeform_);
  member("Time Between Waves", &time_between_waves_);
  member("Time Between Individuals", &time_between_individuals_);
}

bool SwarmSpawn::compile(unsigned int granularity,
                         Evidyon::CompiledRegionSwarmSpawn* description) const {
  description->ai_lifeform_type =
      ai_lifeform_.getReferencedResourceIndex();
  description->spawn_counter_granularity = 
      granularity;
  description->spawn_counter_activate_period =
      time_between_individuals_.getValue() / Evidyon::SWARM_SPAWN_PERIOD;
  description->spawn_counter_reset_value =
     (time_between_waves_.getValue() * granularity) / time_between_individuals_.getValue();

  return true;
}


std::string SwarmSpawn::staticTypeName() {
  return "SwarmSpawn";
}
