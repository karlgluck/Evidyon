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
#include "visualfxinstance_orbitingparticles.h"
#include "visualfxparticledata.h"
#include "visualfxrenderer.h"
#include <memory.h> // memset
#include <assert.h>
#include <math.h>   // cosf/sinf
#include "common/randf.h"


namespace Evidyon {
namespace VisualFX {



  
//----[  VisualFXInstance_OrbitingParticles  ]---------------------------------
VisualFXInstance_OrbitingParticles::VisualFXInstance_OrbitingParticles(
    const VisualFX_OrbitingParticles* parameters) {
  parameters_ = parameters;
  memset(particles_, 0, sizeof(particles_));
  number_of_particles_ = 0;
  last_generation_time_ = 0.0;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_OrbitingParticles::destroy() {
  VisualFXRenderer* visual_fx_renderer = renderer();
  for (size_t i = 0; i < number_of_particles_; ++i) {
     visual_fx_renderer->releaseParticleData(particles_[i]);
  }
  number_of_particles_ = 0;
}





//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_OrbitingParticles::update(
    double time,
    double time_since_last_update) {
  VisualFXRenderer* visual_fx_renderer = renderer();
  float x, y, z;
  bool binding_alive
    = parameters_->attach_to_target ? getTargetLocation(&x, &y, &z)
                                    : getSourceLocation(&x, &y, &z);
  binding_alive = binding_alive &&
    !(parameters_->attach_to_target ? targetExpired() : sourceExpired());

  // Generate new particles if the binding is still OK
  if (binding_alive) {
    if (time - last_generation_time_ > parameters_->generation_period) {
      int particles_to_generate = parameters_->particles_per_period;
      while (particles_to_generate > 0 &&
             number_of_particles_ < MAX_SIMULTANEOUS_PARTICLES) {
        VisualFXParticleData* data = visual_fx_renderer->acquireParticleData();
        if (!data) break;
        data->orbiting_particle.rendering_particle.y
          = y + parameters_->initial_height;
        data->orbiting_particle.rendering_particle.point_size
          = parameters_->particle_size;
        data->orbiting_particle.rendering_particle.color
          = parameters_->particle_colors[rand()%3];
        data->orbiting_particle.angle_from_source
          = randfAngleRadians();
        data->orbiting_particle.distance_from_source
          = parameters_->initial_distance;
        data->orbiting_particle.dying_time
          = time + parameters_->lifetime;
        particles_[number_of_particles_++] = data;
        --particles_to_generate;
      }
      last_generation_time_ = time;
    }
  }

  for (size_t i = 0; i < number_of_particles_; /*++i done in loop*/) {
    VisualFXOrbitingParticleData* data = &particles_[i]->orbiting_particle;

    // Advance the particle's features
    float new_angle
      = data->angle_from_source + parameters_->orbit_speed * time_since_last_update;
    float new_height
      = data->rendering_particle.y + parameters_->rising_speed * time_since_last_update;
    float new_distance
      = data->distance_from_source + parameters_->spread_speed * time_since_last_update;

    // Move this particle
    data->rendering_particle.y = new_height;
    data->rendering_particle.x = x + cosf(new_angle) * new_distance;
    data->rendering_particle.z = z + sinf(new_angle) * new_distance;
    data->distance_from_source = new_distance;
    data->angle_from_source = new_angle;

    // Decay the particle
    double lifetime_left = data->dying_time - time;
    bool particle_dying = lifetime_left < 0.0;
    bool particle_dead
      = particle_dying && (data->rendering_particle.point_size <= 0.0f);

    // Does the particle exist?
    if (particle_dead) {
      visual_fx_renderer->releaseParticleData(particles_[i]);
      particles_[i] = particles_[--number_of_particles_];
      particles_[number_of_particles_] = NULL;
    } else if (particle_dying) {
      data->rendering_particle.point_size
        = parameters_->particle_size + parameters_->decay_rate * lifetime_left;
      ++i;
    } else {
      ++i; // everything's fine; move to the next particle
    }
  }

  return binding_alive || number_of_particles_ > 0;
}





//----[  render  ]----------------------------------------------------
void VisualFXInstance_OrbitingParticles::render(
    Texture::TextureIndex texture) {
  assert(parameters_->particle_texture == texture);
  if (hidden()) return; // don't draw if the particles can't be seen
  VisualFXRenderer* visual_fx_renderer = renderer();
  for (size_t i = 0; i < number_of_particles_; ++i) {
    VisualFXOrbitingParticleData* data = &particles_[i]->orbiting_particle;
    visual_fx_renderer->renderParticle(&data->rendering_particle);
  }
}





}
}
