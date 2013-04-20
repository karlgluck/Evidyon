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
#ifndef __SHARED_WORLD_TOOLS_SWARMSPAWN_H__
#define __SHARED_WORLD_TOOLS_SWARMSPAWN_H__
#pragma once

#include <dc/integer>
#include <dc/float>
#include "shared/lifeformai/tools/lifeformai.h"
#include "shared/lifeformai/lifeformaiindex.h"

namespace Evidyon {
namespace World {
namespace Tools {


  
//----[  SwarmSpawn  ]---------------------------------------------------------
class SwarmSpawn : public dc::dcResource<SwarmSpawn> {
public:
  static std::string staticTypeName() { return "SwarmSpawn"; }
public:
  struct Data {
    static const int NUMBER_OF_LIFEFORM_AI = 16;
    Evidyon::LifeformAI::LifeformAIIndex lifeform_ai[NUMBER_OF_LIFEFORM_AI];
    float chance_of_appearing;
    int lowest_level;
    int highest_level;
    int number_to_choose;
  };
public:
    SwarmSpawn();
    // returns true if there are lifeforms specified
    bool compile(Data* data);
    void addLifeformAIReferences(dc::dcGenericResource::Array& elements);
private:
  dc::dcList<Evidyon::Tools::LifeformAIReference> lifeform_ai_;
  dc::dcFloat chance_of_appearing_;
  dc::dcInteger lowest_level_, highest_level_, number_to_choose_;
};

}
}
}

#endif