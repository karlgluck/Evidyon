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
#include "geoniteburst.h"
#include "shared/magic/tools/magiccompiler.h"
#include <assert.h>



namespace Evidyon {
namespace World {
namespace Tools {



//----[  GeoniteBurst  ]-------------------------------------------------------
GeoniteBurst::GeoniteBurst() {
  member("Location", &location_);
  member("Burst Magic", &burst_magic_);
  member("Burst Geonite Cost", &burst_geonite_cost_);
}



//----[  compile  ]------------------------------------------------------------
void GeoniteBurst::compile(Magic::Tools::MagicCompiler* compiler,
                           int* x,
                           int* y,
                           Magic::MagicIndex* magic_index,
                           Geosid::Geonite* geonite_cost) {
  assert(compiler);
  assert(magic_index);
  assert(geonite_cost);
  float fx, fy;
  location_.get(&fx,&fy);
  *x = int(fx);
  *y = int(fy);
  Magic::Tools::Magic* magic = burst_magic_.getImplementation();
  *magic_index
    = magic ? magic->compile(compiler) : Magic::INVALID_MAGIC_INDEX;
  *geonite_cost = burst_geonite_cost_.getValue();
}



}
}
}

