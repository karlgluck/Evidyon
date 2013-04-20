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
#include "visualfx_orbitingparticles.h"
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_orbitingparticles.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


dc::dcClass<VisualFX_OrbitingParticles>::Implements<VisualFX>
  VisualFX_OrbitingParticles::implementsVisualFX;



//----[  VisualFX_OrbitingParticles  ]-----------------------------------------
VisualFX_OrbitingParticles::VisualFX_OrbitingParticles() {
  member("Particle Texture", &particle_texture_);
  member("Particle Size", &particle_size_);
  member("Color [0]", &colors_[0]);
  member("Color [1]", &colors_[1]);
  member("Color [2]", &colors_[2]);
  member("Visible Time", &visible_time_);
  member("Density", &density_);
  member("Particls per Period", &particles_per_period_);
  member("Decay Time", &decay_time_);
  member("Initial Distance", &initial_distance_);
  member("Initial Height", &initial_height_);
  member("Rising Speed", &rising_speed_);
  member("Spread Speed", &spread_speed_);
  member("Orbit Speed", &orbit_speed_);
  member("Attach to Target", &attach_to_target_);

  particles_per_period_.setValue(1);
}



//----[  compile  ]------------------------------------------------------------
void VisualFX_OrbitingParticles::compile(
    Evidyon::VisualFX::VisualFX* description) {
  description->type = Evidyon::VisualFX::VisualFX::ORBITING_PARTICLES;

  if (particles_per_period_.getValue() < 1) {
    particles_per_period_.setValue(1);
  }

  Evidyon::VisualFX::VisualFX_OrbitingParticles* this_description
    = &description->orbiting_particles;
  this_description->particle_texture
    = particle_texture_.getReferencedResourceIndex();
  this_description->particle_size = particle_size_.getValue();
  this_description->particle_colors[0] = colors_[0].getD3DColor();
  this_description->particle_colors[1] = colors_[1].getD3DColor();
  this_description->particle_colors[2] = colors_[2].getD3DColor();
  this_description->lifetime
    = visible_time_.getValue() - decay_time_.getValue();
  this_description->generation_period
    = visible_time_.getValue() * particles_per_period_.getValue()
      / float(density_.getValue());
  this_description->particles_per_period = particles_per_period_.getValue();
  this_description->decay_rate
    = particle_size_.getValue() / decay_time_.getValue();
  this_description->initial_distance = initial_distance_.getValue();
  this_description->initial_height = initial_height_.getValue();
  this_description->rising_speed = rising_speed_.getValue();
  this_description->spread_speed = spread_speed_.getValue();
  this_description->orbit_speed = D3DXToRadian(orbit_speed_.getValue());
  this_description->attach_to_target = attach_to_target_.getValue();
}


}
}
}