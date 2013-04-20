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