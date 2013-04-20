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

#include "visualfx_particletrail.h"
#include "../shared/compiledspecialfx.h"




/// This class implements a special effect
dcClass<VisualFX_ParticleTrail>::Implements<VisualFX> VisualFX_ParticleTrail::implementsVisualFX;


//------------------------------------------------------------------------------------------------
// Name: ParticleTrailEffectVisualization
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VisualFX_ParticleTrail::VisualFX_ParticleTrail() {
  member("Image", &myImage);
  member("Particle Color - 0", &myParticleColor[0]);
  member("Particle Color - 1", &myParticleColor[1]);
  member("Particle Color - 2", &myParticleColor[2]);
  member("Target Source", &target_source_);
  member("Particle Size", &myParticleSize);
  member("Duration", &myDuration);
  member("Particles Per Period", &myParticlesPerPeriod);
  member("Particle Total Lifetime", &myTotalLifetime);
  member("Particle Decay Time", &myDecayTime);
  member("Density", &myParticleDensity );
  member("Rising Speed", &myRisingSpeed);
  member("Radial Velocity", &myRadialVelocity);
  member("Initial Radial Distance", &myInitialRadialDistance);
  member("Initial Height", &myInitialHeight);
  member("Initial Vertical Spread (deg)", &myInitialVerticalSpread);

  target_source_.setValue(true);
}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool VisualFX_ParticleTrail::compileForClient(Evidyon::CompiledVisualFXParams* params) const {
  params->type = target_source_.getValue() ?
      Evidyon::CompiledVisualFXParams::TRAILING_PARTICLES_SOURCE :
      Evidyon::CompiledVisualFXParams::TRAILING_PARTICLES_TARGET;
  CompiledVisualFXParams_TrailingParticles* description = &params->trailing_particles;

  float decay_time = myDecayTime.getValue();
  {
    float lifetime = myTotalLifetime.getValue();
    decay_time = min(decay_time, lifetime);
  }

  description->particle_sfx_image = myImage.getReferencedResourceIndex();
  description->duration = myDuration.getValue();
  for (int color = 0; color < 3; ++color) {
    description->particleColors[color] = myParticleColor[color].getD3DColor();
  }
  description->particlesPerPeriod = myParticlesPerPeriod.getValue();
  description->lifetime = myTotalLifetime.getValue() - decay_time;
  description->decayRate = myParticleSize.getValue() / decay_time;
  params->orbiting_particles.spawnPeriod =
      (myTotalLifetime.getValue() * myParticlesPerPeriod.getValue()) /
       myParticleDensity.getValue();
  description->risingSpeed = myRisingSpeed.getValue();
  description->radialVelocity = myRadialVelocity.getValue();
  description->initial_radial_distance = myInitialRadialDistance.getValue();
  description->initial_height = myInitialHeight.getValue();
  description->half_vertical_spread_angle =
      D3DXToRadian(myInitialVerticalSpread.getValue()) / 2.0f;
  description->particleSize = myParticleSize.getValue();

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string VisualFX_ParticleTrail::staticTypeName() {
    return "VisualFX_ParticleTrail";
}