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
#include "visualfx_beam.h"
#include <assert.h>
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_beam.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


dc::dcClass<VisualFX_Beam>::Implements<VisualFX>
  VisualFX_Beam::implementsVisualFX;


  
//----[  VisualFX_Beam  ]------------------------------------------------------
VisualFX_Beam::VisualFX_Beam() {
  member("End Particles Texture", &end_particles_texture_);
  member("End Particles Color", &end_particles_color_);
  member("Beam Texture", &beam_texture_);
  member("Beam Color", &beam_color_);
  member("End Particle Size (Minimum Intensity)", &end_particle_size_min_intensity_);
  member("End Particle Size (Maximum Intensity)", &end_particle_size_max_intensity_);
  member("Beam Width (Minimum Intensity)", &beam_width_min_intensity_);
  member("Beam Width (Maximum Intensity)", &beam_width_max_intensity_);

  end_particle_size_min_intensity_.setValue(0.5f);
  end_particle_size_max_intensity_.setValue(1.0f);
  beam_width_min_intensity_.setValue(0.5f);
  beam_width_max_intensity_.setValue(1.0f);
  end_particles_color_.setD3DColor(D3DCOLOR_XRGB(255,255,255));
  beam_color_.setD3DColor(D3DCOLOR_XRGB(255,255,255));
}




//----[  compile  ]------------------------------------------------------------
void VisualFX_Beam::compile(Evidyon::VisualFX::VisualFX* description) {
  Evidyon::VisualFX::VisualFX_Beam* beam = &description->beam;
  description->type = Evidyon::VisualFX::VisualFX::BEAM;
  beam->end_particles_texture = end_particles_texture_.getReferencedResourceIndex();
  beam->end_particles_color = end_particles_color_.getD3DColor();
  beam->beam_texture = beam_texture_.getReferencedResourceIndex();
  beam->beam_color = beam_color_.getD3DColor();
  beam->end_particle_base_size = end_particle_size_min_intensity_.getValue();
  beam->end_particle_intensity_size
    = end_particle_size_max_intensity_.getValue()
      - end_particle_size_min_intensity_.getValue();
  beam->beam_base_width = beam_width_min_intensity_.getValue();
  beam->beam_intensity_width
    = beam_width_max_intensity_.getValue()
      - beam_width_min_intensity_.getValue();
}


}
}
}