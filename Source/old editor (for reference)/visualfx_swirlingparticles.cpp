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
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"
#include "visualfx_swirlingparticles.h"


#include "../shared/compiledspecialfx.h"



/// This class implements a special effect
dcClass<VisualFX_SwirlingParticles>::Implements<VisualFX> VisualFX_SwirlingParticles::implements_visual_fx;




//------------------------------------------------------------------------------------------------
// Name: VisualFX_SwirlingParticles
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VisualFX_SwirlingParticles::VisualFX_SwirlingParticles() {
  member("Image", &particle_image_);
  member("Duration", &duration_);
  member("Target Source", &target_source_);
  member("Particle Color - 0", &particle_colors_[0]);
  member("Particle Color - 1", &particle_colors_[1]);
  member("Particle Color - 2", &particle_colors_[2]);
  member("Number of Rings", &number_of_rings_);
  member("Minimum Ring Height", &min_ring_height_);
  member("Maximum Ring Height", &max_ring_height_);
  member("Particles per Ring", &particles_per_ring_);
  member("Trail Length", &particle_trail_length_);
  member("Orbit Radius", &orbit_radius_);
  member("Minimum Speed (deg/sec)", &minimum_speed_);
  member("Maximum Speed (deg/sec)", &maximum_speed_);
  member("Particle Size", &particle_size_);

  target_source_.setValue(true);

  for (int i = 0; i < 3; ++i) {
    particle_colors_[i].getR()->setValue(255);
    particle_colors_[i].getG()->setValue(255);
    particle_colors_[i].getB()->setValue(255);
  }
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool VisualFX_SwirlingParticles::compileForClient(Evidyon::CompiledVisualFXParams* params) const {
  params->swirling_particles.particle_sfx_image = particle_image_.getReferencedResourceIndex();
  params->swirling_particles.duration = duration_.getValue();
  params->type = target_source_.getValue() ?
      Evidyon::CompiledVisualFXParams::SWIRLING_PARTICLES_SOURCE :
      Evidyon::CompiledVisualFXParams::SWIRLING_PARTICLES_TARGET;
  for (int color = 0; color < 3; ++color) {
    params->swirling_particles.particle_colors[color] = particle_colors_[color].getD3DColor();
  }

  params->swirling_particles.number_of_rings = number_of_rings_.getValue();
  params->swirling_particles.lowest_ring_height = min_ring_height_.getValue();
  params->swirling_particles.added_height_per_ring = (max_ring_height_.getValue() - min_ring_height_.getValue())/number_of_rings_.getValue();
  params->swirling_particles.particles_per_ring = particles_per_ring_.getValue();
  params->swirling_particles.particle_trail_length = particle_trail_length_.getValue();
  params->swirling_particles.orbit_radius = orbit_radius_.getValue();
  params->swirling_particles.min_speed = D3DXToRadian(minimum_speed_.getValue());
  params->swirling_particles.speed_range = D3DXToRadian(maximum_speed_.getValue() - minimum_speed_.getValue());
  params->swirling_particles.particle_size = particle_size_.getValue();

  // Success
  return true;
}

//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string VisualFX_SwirlingParticles::staticTypeName()
{
    return "VisualFX_SwirlingParticles";
}


