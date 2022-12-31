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
#ifndef __LIFEFORMAIINHABITANTSPAWN_H__
#define __LIFEFORMAIINHABITANTSPAWN_H__
#pragma once



#include <dc/integer>
#include <dc/table>
#include "shared/lifeformai/tools/lifeformai.h"
#include "location.h"

namespace Evidyon {
namespace World {
namespace Tools {

//----[  LifeformAIInhabitantSpawn  ]------------------------------------------
class LifeformAIInhabitantSpawn
    : public dc::dcResource<LifeformAIInhabitantSpawn> {
public:
  static std::string staticTypeName() { return "LifeformAIInhabitantSpawn"; }
public:
  LifeformAIInhabitantSpawn();
  void compile(MapCompilationContext* context) const;

  void set(int x, int y, Evidyon::Tools::LifeformAIElement* element);

  // Returns 'true' if this spawn inhabitant is at the given coordinates
  bool locatedAt(int x, int y) const;

private:

  // Where to spawn this inhabitant
  Location location_;

  // The inhabitant itself
  Evidyon::Tools::LifeformAIReference lifeform_ai_;
};

}
}
}

#endif