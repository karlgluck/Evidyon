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
#include "visualfx_scenery.h"
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_scenery.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


dc::dcClass<VisualFX_Scenery>::Implements<VisualFX>
  VisualFX_Scenery::implementsVisualFX;



//----[  VisualFX_Scenery  ]---------------------------------------------------
VisualFX_Scenery::VisualFX_Scenery() {
  member("Scenery", &scenery_);
  member("Turn to Follow Path?", &turn_to_follow_path_);
}


//----[  compile  ]------------------------------------------------------------
void VisualFX_Scenery::compile(
    Evidyon::VisualFX::VisualFX* description) {
  description->type = Evidyon::VisualFX::VisualFX::SCENERY;

  Evidyon::VisualFX::VisualFX_Scenery* this_description
    = &description->scenery;
  this_description->scenery_index = scenery_.getReferencedResourceIndex();
  this_description->turn_to_follow_path = turn_to_follow_path_.getValue();
}


}
}
}