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
#include "actorid.h"


namespace Evidyon {
namespace Actor {



  
//----[  PackActorID  ]--------------------------------------------------------
ActorID PackActorID(unsigned int incremented_value,
                    unsigned int actor_structure_mempool_index,
                    ActorTemplateIndex actor_template) {
  actor_template
    |= actor_structure_mempool_index << ACTORID_TEMPLATE_BITS;
  actor_template
    |= incremented_value << (ACTORID_TEMPLATE_BITS + ACTORID_ACTOR_INDEX_BITS);
  return static_cast<ActorID>(actor_template);
}



//----[  UnpackActorID  ]------------------------------------------------------
void UnpackActorID(ActorID actor_id,
                   ActorTemplateIndex* actor_template) {
  static const unsigned int TEMPLATE_MASK =
    ((~(unsigned)(0L))>>(sizeof(unsigned int)*8 - ACTORID_TEMPLATE_BITS));
  *actor_template = actor_id & TEMPLATE_MASK;
}

}
}

