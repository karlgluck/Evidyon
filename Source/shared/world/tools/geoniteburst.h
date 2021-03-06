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
#ifndef __SHARED_WORLD_TOOLS_GEONITEBURST_H__
#define __SHARED_WORLD_TOOLS_GEONITEBURST_H__
#pragma once

#include "shared/magic/tools/magic.h"
#include <dc/integer>
#include "shared/magic/magicindex.h"
#include "shared/geosid/geonite.h"
#include "location.h"

namespace Evidyon {
namespace Magic {
namespace Tools {
class MagicCompiler;
}
}
}


namespace Evidyon {
namespace World {
namespace Tools {


//----[  GeoniteBurst  ]-------------------------------------------------------
class GeoniteBurst : public dc::dcResource<GeoniteBurst> {
public:
  static std::string staticTypeName() { return "GeoniteBurst"; }

public:
  GeoniteBurst();
  void compile(Magic::Tools::MagicCompiler* compiler,
               int* x,
               int* y,
               Magic::MagicIndex* magic_index,
               Geosid::Geonite* geonite_cost);

private:
  Location location_;
  Evidyon::Tools::MagicInterface burst_magic_;
  dc::dcInteger burst_geonite_cost_;
};


}
}
}

#include <dc/list>

namespace Evidyon {
namespace World {
namespace Tools {
typedef dc::dcList<GeoniteBurst> GeoniteBurstList;
typedef dc::dcList<GeoniteBurst>::Element GeoniteBurstElement;
//typedef dc::dcList<GeoniteBurst>::Reference GeoniteBurstReference;
}
}
}

#endif