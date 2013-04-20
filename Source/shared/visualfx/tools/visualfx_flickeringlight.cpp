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
#include "visualfx_flickeringlight.h"
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_flickeringlight.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


dc::dcClass<VisualFX_FlickeringLight>::Implements<VisualFX>
  VisualFX_FlickeringLight::implementsVisualFX;


//----[  VisualFX_FlickeringLight  ]-------------------------------------------
VisualFX_FlickeringLight::VisualFX_FlickeringLight() {
  member("Color [0]", &colors_[0]);
  member("Color [1]", &colors_[1]);
  member("Color [2]", &colors_[2]);
}


//----[  compile  ]------------------------------------------------------------
void VisualFX_FlickeringLight::compile(
    Evidyon::VisualFX::VisualFX* description) {
  description->type = Evidyon::VisualFX::VisualFX::FLICKERING_LIGHT;

  Evidyon::VisualFX::VisualFX_FlickeringLight* this_description
    = &description->flickering_light;
  this_description->colors[0] = colors_[0].getD3DColor();
  this_description->colors[1] = colors_[1].getD3DColor();
  this_description->colors[2] = colors_[2].getD3DColor();
}


}
}
}