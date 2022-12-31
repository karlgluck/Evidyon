//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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