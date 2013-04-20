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
#include "visualfx_fireflycloud.h"
#include "shared/visualfx/visualfx.h"
#include "shared/visualfx/visualfx_fireflycloud.h"


namespace Evidyon {
namespace VisualFX {
namespace Tools {


dc::dcClass<VisualFX_FireflyCloud>::Implements<VisualFX>
  VisualFX_FireflyCloud::implementsVisualFX;


  
//----[  VisualFX_FireflyCloud  ]----------------------------------------------
VisualFX_FireflyCloud::VisualFX_FireflyCloud() {
  member("Particle Texture", &particle_texture_);
  member("Particle Size", &particle_size_);
  member("Color [0]", &colors_[0]);
  member("Color [1]", &colors_[1]);
  member("Color [2]", &colors_[2]);
  member("Visible Time", &visible_time_);
  member("Density", &density_);
  member("Particles per Period", &particles_per_period_);
  member("Percent of Visible Time Fading", &fading_time_percent_);
  member("Distance Range", &distance_range_);
  member("Minimum Speed", &minimum_speed_);
  member("Maximum Speed", &maximum_speed_);
  member("Attach to Target", &attach_to_target_);

  particles_per_period_.setValue(1);
}




//----[  compile  ]------------------------------------------------------------
void VisualFX_FireflyCloud::compile(Evidyon::VisualFX::VisualFX* description) {
  description->type = Evidyon::VisualFX::VisualFX::FIREFLY_CLOUD;

  if (particles_per_period_.getValue() < 1) {
    particles_per_period_.setValue(1);
  }
  if (fading_time_percent_.getValue() < 0.0f) {
    fading_time_percent_.setValue(0.0f);
  }
  float fading_time
    = fading_time_percent_.getValue()/100.0f/2.0f * visible_time_.getValue();

  Evidyon::VisualFX::VisualFX_FireflyCloud* this_description
    = &description->firefly_cloud;
  this_description->particle_texture
    = particle_texture_.getReferencedResourceIndex();
  this_description->particle_size = particle_size_.getValue();
  this_description->particle_colors[0] = colors_[0].getD3DColor();
  this_description->particle_colors[1] = colors_[1].getD3DColor();
  this_description->particle_colors[2] = colors_[2].getD3DColor();
  this_description->lifetime = visible_time_.getValue() - 2*fading_time;
  this_description->generation_period
    = visible_time_.getValue() * particles_per_period_.getValue()
      / float(density_.getValue());
  this_description->particles_per_period = particles_per_period_.getValue();
  this_description->fade_rate
    = particle_size_.getValue() / fading_time;
  this_description->distance_range = distance_range_.getValue();
  this_description->base_speed = minimum_speed_.getValue();
  this_description->speed_random_range
    = maximum_speed_.getValue() - minimum_speed_.getValue();
  this_description->attach_to_target = attach_to_target_.getValue();
}



}
}
}