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