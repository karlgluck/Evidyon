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
#include "mapcompilationcontext.h"
#include "common/null.h"
#include <assert.h>


namespace Evidyon {
namespace World {
namespace Tools {


  


  
//----[  atOrDefault  ]--------------------------------------------------------
MapLocationData* MapCompilationContext::atOrDefault(int x, int y) {
  accessed = true;
  return (x < 0 || y < 0 || x >= width || y >= height)
    ? &default_location : &locations[y*width+x];
}



//----[  atOrNull  ]-----------------------------------------------------------
MapLocationData* MapCompilationContext::atOrNull(int x, int y) {
  accessed = true;
  return (x < 0 || y < 0 || x >= width || y >= height)
    ? NULL : &locations[y*width+x];
}




//----[  addTrigger  ]---------------------------------------------------------
Trigger::TriggerIndex MapCompilationContext::addTrigger(
    const Trigger::Trigger* trigger) {
  triggers.push_back(*trigger);
  return triggers.size() - 1;
}



//----[  compileTriggers  ]----------------------------------------------------
size_t MapCompilationContext::compileTriggers(Trigger::Trigger* triggers,
                                              size_t buffer_length) {
  assert(this->triggers.size() <= buffer_length);
  size_t index = 0;
  for (Triggers::iterator i = this->triggers.begin();
                          i != this->triggers.end();
                        ++i, ++index) {
    memcpy(&triggers[index], &*i, sizeof(Trigger::Trigger));
  }
  return index;
}


}
}
}