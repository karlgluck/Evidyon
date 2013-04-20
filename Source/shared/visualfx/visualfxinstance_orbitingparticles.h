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
#ifndef __VISUALFXINSTANCE_ORBITINGPARTICLES_H__
#define __VISUALFXINSTANCE_ORBITINGPARTICLES_H__
#pragma once


#include "visualfxinstance.h"
#include "visualfx_orbitingparticles.h"

namespace Evidyon {
namespace VisualFX {
union VisualFXParticleData;


//----[  VisualFXInstance_OrbitingParticles  ]---------------------------------
class VisualFXInstance_OrbitingParticles : public VisualFXInstance {
  static const int MAX_SIMULTANEOUS_PARTICLES = 256;

public:
  VisualFXInstance_OrbitingParticles(
    const VisualFX_OrbitingParticles* parameters);
  virtual void destroy();
  virtual bool update(double time,
                      double time_since_last_update);
  virtual void render(Texture::TextureIndex texture);

private:
  const VisualFX_OrbitingParticles* parameters_;
  VisualFXParticleData* particles_[MAX_SIMULTANEOUS_PARTICLES];
  size_t number_of_particles_;
  double last_generation_time_;
};


}
}

#endif