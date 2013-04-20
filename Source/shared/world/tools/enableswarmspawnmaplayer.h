//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#ifndef __ENABLESWARMSPAWNMAPLAYER_H__
#define __ENABLESWARMSPAWNMAPLAYER_H__
#pragma once


#include "maplayer.h"
#include "mapmaskcolorreference.h"

namespace Evidyon {
namespace World {
namespace Tools {

//----[  EnableSwarmSpawnMapLayer  ]------------------------------------------------------
// Causes the masked areas to spawn monsters.
class EnableSwarmSpawnMapLayer : public dc::dcResource<EnableSwarmSpawnMapLayer,MapLayer> {
public:
  static std::string staticTypeName() { return "EnableSwarmSpawnMapLayer"; }
  static dc::dcClass<EnableSwarmSpawnMapLayer>::Implements<MapLayer> implements_map_layer_;
public:
  EnableSwarmSpawnMapLayer();
  virtual void compile(const RECT* area,
                       MapCompilationContext* compilation_context);
  void setArea(dc::dcList<MapMask>::Element* map_mask, D3DCOLOR color);
private:
  MapMaskColorReference mask_area_;
};

}
}
}

#endif