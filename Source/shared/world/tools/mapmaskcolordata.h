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
#ifndef __SHARED_WORLD_TOOLS_TARGETMONSTERLEVELAREAS_H__
#define __SHARED_WORLD_TOOLS_TARGETMONSTERLEVELAREAS_H__
#pragma once

#include "shared/world/navigability.h"
#include <dc/enum>
#include <dc/integer>
#include <dcx/d3dcolor>
#include "shared/zone/tools/zone.h"
#include "shared/zone/zoneindex.h"
#include "swarmspawn.h"

namespace Evidyon {
namespace World {
namespace Tools {
class MapMask;
}
}
}

namespace Evidyon {
namespace World {
namespace Tools {


  
//----[  MapMaskColorData  ]---------------------------------------------------
class MapMaskColorData : public dc::dcResource<MapMaskColorData> {
public:
  static std::string staticTypeName() { return "MapMaskColorData"; }
public:
  MapMaskColorData();

  dcx::dcxD3DColor* getColor();
  D3DCOLOR getD3DColor() const;

  // Returns 'true' if the navigability is not invalid
  bool getNavigability(Navigability* navigability) const;

  // Returns 'true' if the swarm level range is set.  The range returned
  // is never zero.
  bool getSwarmLevel(int* min_level, int* level_range) const;

  // Returns the number of swarms filled in the output array.
  size_t getSwarms(SwarmSpawn::Data* swarm_spawns, size_t size) const;

  bool getZone(Zone::ZoneIndex* zone) const;

private:
  dc::dcEnum<Navigability> navigability_;
  dcx::dcxD3DColor color_;
  dc::dcInteger monster_swarm_level_min_;
  dc::dcInteger monster_swarm_level_max_;
  dc::dcList<SwarmSpawn> monster_swarms_;
  Evidyon::Tools::ZoneReference zone_;
};



}
}
}

#endif