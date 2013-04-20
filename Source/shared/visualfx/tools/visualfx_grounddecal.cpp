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
#include "visualfx_grounddecal.h"
#include <assert.h>
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_grounddecal.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {



dc::dcClass<VisualFX_GroundDecal>::Implements<VisualFX>
  VisualFX_GroundDecal::implementsVisualFX;



//----[  VisualFX_GroundDecal  ]-----------------------------------------------
VisualFX_GroundDecal::VisualFX_GroundDecal() {
  member("Texture", &texture_);
  member("Color [0]", &colors_[0]);
  member("Color [1]", &colors_[1]);
  member("Color [2]", &colors_[2]);
  member("Minimum Size", &min_size_);
  member("Maximum Size", &max_size_);
  member("Generation Period", &generate_period_);
  member("Decal Total Visible Time", &total_visible_time_);
  member("Decal Fade Time", &fade_time_);

  min_size_.setValue(1.0f);
  max_size_.setValue(1.0f);
  colors_[0].setD3DColor(D3DCOLOR_XRGB(255,255,255));
  colors_[1].setD3DColor(D3DCOLOR_XRGB(255,255,255));
  colors_[2].setD3DColor(D3DCOLOR_XRGB(255,255,255));
}




//----[  compile  ]------------------------------------------------------------
void VisualFX_GroundDecal::compile(Evidyon::VisualFX::VisualFX* description) {
  Evidyon::VisualFX::VisualFX_GroundDecal* ground_decal
      = &description->ground_decal;
  description->type = Evidyon::VisualFX::VisualFX::GROUND_DECAL;
  ground_decal->texture = texture_.getReferencedResourceIndex();
  ground_decal->random_size_base = min_size_.getValue();
  ground_decal->random_size_range = max_size_.getValue() - min_size_.getValue();
  ground_decal->colors[0] = colors_[0].getD3DColor();
  ground_decal->colors[1] = colors_[1].getD3DColor();
  ground_decal->colors[2] = colors_[2].getD3DColor();
  ground_decal->generate_period = generate_period_.getValue();
  ground_decal->decal_lifetime
    = total_visible_time_.getValue() - fade_time_.getValue();
  ground_decal->decal_fade_rate = 255.0f / fade_time_.getValue();
}


}
}
}
