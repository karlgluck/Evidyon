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
#ifndef __LOCATION_H__
#define __LOCATION_H__
#pragma once



#include <dc/integer>

namespace Evidyon {
namespace World {
namespace Tools {

struct MapCompilationContext;
struct MapLocationData;

//----[  Location  ]-----------------------------------------------------------
class Location : public dc::dcResource<Location> {
public:
  static std::string staticTypeName() { return "Location"; }

public:
  Location();

  // Obtains the data for the referenced coordinate from the map context.  If
  // this location references a cell outside of the map's boundaries, this
  // method will return NULL.
  // 'context' must not be NULL itself.
  MapLocationData* getMapLocationData(MapCompilationContext* context) const;

  // Changes this location's coordinates
  void set(int x, int y);

  // Returns 'true' if this location is at the provided coordinates
  bool equals(int x, int y) const;

  void get(float* x, float* y) const;

private:
  dc::dcInteger x_, y_;
};

}
}
}

#endif