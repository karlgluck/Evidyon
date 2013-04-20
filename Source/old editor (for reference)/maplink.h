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
#ifndef __MAPLINK_H__
#define __MAPLINK_H__
#pragma once

#include <dc/list>
#include "maplocationspecialfunction.h"
#include "location.h"
#include "atlas.h"

namespace Evidyon {

struct CompiledMapLocationSpecialFunction;

}


class MapLink : public dc::dcResource<MapLink,MapLocationSpecialFunction> {
public:
  MapLink();
  virtual bool compileForServer(Evidyon::CompiledMapLocationSpecialFunction* description);

public:
  static std::string staticTypeName();

private:
  Location source_location_;          // where to jump from
  dc::dcList<Atlas>::Reference dest_map_; // the map to jump to (uses containing map if null)
  Location dest_location_;            // to the location to jump to

private:
  static dc::dcClass<MapLink>::Implements<MapLocationSpecialFunction> implementsSpecialFunction;
};





#endif