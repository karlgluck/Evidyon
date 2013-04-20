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
#include "swarmspawn.h"
#include "mapcompilationcontext.h"
#include "maplocationdata.h"

namespace Evidyon {
namespace World {
namespace Tools {



  
//----[  SwarmSpawn  ]---------------------------------------------------------
SwarmSpawn::SwarmSpawn() {
  member("Lifeform Options", &lifeform_ai_);
  member("Chance of Appearing (%)", &chance_of_appearing_);
  member("Lowest Target Level", &lowest_level_);
  member("Highest Target Level", &highest_level_);
  member("Number to Choose", &number_to_choose_);

  chance_of_appearing_.setValue(100.0f);
  lowest_level_.setValue(1);
  highest_level_.setValue(4);
  number_to_choose_.setValue(-1);
}



//----[  compile  ]------------------------------------------------------------
bool SwarmSpawn::compile(Data* data) {
  const dc::dcGenericResource::Array& lifeform_ais = lifeform_ai_.getMembers();
  size_t number_of_lifeform_ais = lifeform_ais.size();
  if (number_of_lifeform_ais > 0) {
    for (size_t i = 0; i < Data::NUMBER_OF_LIFEFORM_AI; ++i) {
      typedef dc::dcList<Evidyon::Tools::LifeformAIReference>::Element Element;
      Element* element = (Element*)lifeform_ais.at(i%number_of_lifeform_ais);
      data->lifeform_ai[i] = element->getReferencedResourceIndex();
    }
    data->chance_of_appearing = chance_of_appearing_.getValue() / 100.0f;
    data->lowest_level = lowest_level_.getValue();
    data->highest_level = highest_level_.getValue();
    data->number_to_choose = number_to_choose_.getValue() <= 0 ? number_of_lifeform_ais : number_to_choose_.getValue();
    return true;
  } else {
    for (size_t i = 0; i < Data::NUMBER_OF_LIFEFORM_AI; ++i) {
      data->lifeform_ai[i] = LifeformAI::INVALID_LIFEFORM_AI_INDEX;
    }
    data->chance_of_appearing = 0.0f;
    data->lowest_level = -1;
    data->highest_level = -2;
    data->number_to_choose = -1;
    return false;
  }
}



//----[  addLifeformAIReferences  ]--------------------------------------------
void SwarmSpawn::addLifeformAIReferences(dc::dcGenericResource::Array& elements) {
  dc::dcGenericResource::Array::iterator i = elements.begin();
  for (; i != elements.end(); ++i) {
    dc::dcGenericResource* resource = *i;
    typedef dc::dcList<Evidyon::Tools::LifeformAIReference>::Element Element;
    Element* element
      = (Element*)lifeform_ai_.generateElement(resource->getName());
    element->setGenericReferencedResource(resource);
  }
}


}
}
}

