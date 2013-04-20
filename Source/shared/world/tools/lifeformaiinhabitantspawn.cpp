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
#include "lifeformaiinhabitantspawn.h"
#include "mapcompilationcontext.h"
#include "maplocationdata.h"

namespace Evidyon {
namespace World {
namespace Tools {



  
//----[  LifeformAIInhabitantSpawn  ]------------------------------------------
LifeformAIInhabitantSpawn::LifeformAIInhabitantSpawn() {
  member("Location", &location_);
  member("LifeformAI", &lifeform_ai_);
}



//----[  compile  ]------------------------------------------------------------
void LifeformAIInhabitantSpawn::compile(MapCompilationContext* context) const {
  MapLocationData* location = location_.getMapLocationData(context);
  const dc::dcTable<LifeformAI::Tools::LifeformAI>::Element* lifeform_ai
      = lifeform_ai_.getReferencedResource();
  if (location && lifeform_ai) {
    location->this_layer.number_of_spawns = 1;
    location->this_layer.spawn[0] = lifeform_ai->getIndex();
    location->this_layer.spawn_intended_level = -1;
    location->this_layer.spawn_type = MapLocationData::SPAWN_INHABITANT;
  }
}



//----[  set  ]----------------------------------------------------------------
void LifeformAIInhabitantSpawn::set(int x,
                                    int y,
                                    Evidyon::Tools::LifeformAIElement* element) {
  location_.set(x, y);
  lifeform_ai_.setReferencedResource(element);
}


//----[  locatedAt  ]----------------------------------------------------------
bool LifeformAIInhabitantSpawn::locatedAt(int x, int y) const {
  return location_.equals(x, y);
}


}
}
}