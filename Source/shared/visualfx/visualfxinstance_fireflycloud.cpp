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
#include "visualfxinstance_fireflycloud.h"
#include "visualfx_fireflycloud.h"
#include "shared/visualfx/visualfxrenderer.h"
#include <d3dx9.h>
#include <assert.h>
#include "common/randf.h"


namespace Evidyon {
namespace VisualFX {

  
//----[  VisualFXInstance_FireflyCloud  ]---------------------------------
VisualFXInstance_FireflyCloud::VisualFXInstance_FireflyCloud(
    const VisualFX_FireflyCloud* parameters) {
  parameters_ = parameters;
  memset(particles_, 0, sizeof(particles_));
  number_of_particles_ = 0;
  last_generation_time_ = 0.0;
}





//----[  destroy  ]------------------------------------------------------------
void VisualFXInstance_FireflyCloud::destroy() {
  VisualFXRenderer* visual_fx_renderer = renderer();
  for (size_t i = 0; i < number_of_particles_; ++i) {
     visual_fx_renderer->releaseParticleData(particles_[i]);
  }
  number_of_particles_ = 0;
}





//----[  update  ]-------------------------------------------------------------
bool VisualFXInstance_FireflyCloud::update(
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
        float random_angle = randfAngleRadians();
        float random_distance
          = (randf()) * parameters_->distance_range;
        float random_speed
          = parameters_->base_speed
            + (randf()) * parameters_->speed_random_range;
        float random_height
          = (randf()) * 4.0f;
        D3DXVECTOR3 velocity(randfSymmetric(),
                             randfSymmetric(),
                             randfSymmetric());
        D3DXVec3Normalize(&velocity, &velocity);
        D3DXVec3Scale(&velocity, &velocity, random_speed);
        VisualFXParticleData* data = visual_fx_renderer->acquireParticleData();
        if (!data) break;
        data->firefly_cloud_particle.rendering_particle.x
          = x + cosf(random_angle) * random_distance;
        data->firefly_cloud_particle.rendering_particle.y
          = y + random_height;
        data->firefly_cloud_particle.rendering_particle.z
          = z + sinf(random_angle) * random_distance;
        data->firefly_cloud_particle.rendering_particle.point_size
          = 0.0f;
        data->firefly_cloud_particle.rendering_particle.color
          = parameters_->particle_colors[rand()%3];
        data->firefly_cloud_particle.fading_time
          = time;
        data->firefly_cloud_particle.vx = velocity.x;
        data->firefly_cloud_particle.vy = velocity.y;
        data->firefly_cloud_particle.vz = velocity.z;
        data->firefly_cloud_particle.has_lived = false;
        particles_[number_of_particles_++] = data;
        --particles_to_generate;
      }
      last_generation_time_ = time;
    }
  }

  for (size_t i = 0; i < number_of_particles_; /*++i done in loop*/) {
    VisualFXFireflyCloudParticleData* data
      = &particles_[i]->firefly_cloud_particle;

    // Move this particle
    data->rendering_particle.x += data->vx * time_since_last_update;
    data->rendering_particle.y += data->vy * time_since_last_update;
    data->rendering_particle.z += data->vz * time_since_last_update;

    // Fade the particle
    double fadetime_left = data->fading_time - time;
    bool particle_fading = fadetime_left < 0.0;
    bool has_lived = data->has_lived;
    if (has_lived) {
      bool particle_dead
        = particle_fading &&
          (data->rendering_particle.point_size < 0.0f);

      // Does the particle exist?
      if (particle_dead) {
        visual_fx_renderer->releaseParticleData(particles_[i]);
        particles_[i] = particles_[--number_of_particles_];
        particles_[number_of_particles_] = NULL;
      } else if (particle_fading) {
        data->rendering_particle.point_size
          = parameters_->particle_size
            + parameters_->fade_rate * fadetime_left;
        ++i;
      } else {
        ++i; // everything's fine; move to the next particle
      }
    } else {
      data->rendering_particle.point_size
        = -parameters_->fade_rate * fadetime_left;
      if (data->rendering_particle.point_size >=
          parameters_->particle_size) {
        data->fading_time = time + parameters_->lifetime;
        data->has_lived = true;
        data->rendering_particle.point_size
          = parameters_->particle_size;
      }
      ++i;
    }

  }

  return binding_alive || number_of_particles_ > 0;
}





//----[  render  ]----------------------------------------------------
void VisualFXInstance_FireflyCloud::render(
    Texture::TextureIndex texture) {
  assert(parameters_->particle_texture == texture);
  //if (hidden()) return; // don't draw if the particles can't be seen
  VisualFXRenderer* visual_fx_renderer = renderer();
  for (size_t i = 0; i < number_of_particles_; ++i) {
    VisualFXFireflyCloudParticleData* data
      = &particles_[i]->firefly_cloud_particle;
    visual_fx_renderer->renderParticle(&data->rendering_particle);
  }

}





}
}
