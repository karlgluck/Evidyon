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
#include "location.h"
#include "maplocationdata.h"
#include "mapcompilationcontext.h"
#include "common/null.h"
#include <assert.h>


namespace Evidyon {
namespace World {
namespace Tools {

  
//----[  Location  ]-----------------------------------------------------------
Location::Location() {
  member("x", &x_);
  member("y", &y_);
}




//----[  getMapLocationData  ]-------------------------------------------------
MapLocationData* Location::getMapLocationData(MapCompilationContext* context) const {
  assert(context);
  return context->atOrNull(x_.getValue(), y_.getValue());
}


//----[  set  ]----------------------------------------------------------------
void Location::set(int x, int y) {
  x_.setValue(x);
  y_.setValue(y);
}


//----[  equals  ]-------------------------------------------------------------
bool Location::equals(int x, int y) const {
  return x_.getValue() == x && y_.getValue() == y;
}



//----[  get  ]----------------------------------------------------------------
void Location::get(float* x, float* y) const {
  *x = x_.getValue() + 0.5;
  *y = y_.getValue() + 0.5;
}


}
}
}