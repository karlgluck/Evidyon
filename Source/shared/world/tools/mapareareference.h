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
#ifndef __MAPAREAREFERENCE_H__
#define __MAPAREAREFERENCE_H__
#pragma once



#include <dc/list>
#include <dc/integer>
#include "map.h"

namespace Evidyon {
namespace World {
namespace Tools {

//----[  MapAreaReference  ]---------------------------------------------------
class MapAreaReference : public dc::dcResource<MapAreaReference> {
public:
  static std::string staticTypeName() { return "MapAreaReference"; }
public:
  MapAreaReference();
private:
  dc::dcList<Map>::Reference map_;
  dc::dcInteger top_, left_, bottom_, right_;
};

}
}
}

#endif