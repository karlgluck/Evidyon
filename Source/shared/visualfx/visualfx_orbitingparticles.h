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
#ifndef __VISUALFX_ORBITINGPARTICLES_H__
#define __VISUALFX_ORBITINGPARTICLES_H__
#pragma once


#include "shared/texture/textureindex.h"
#include "common/d3dcolor.h"


namespace Evidyon {
namespace VisualFX {


//----[  VisualFX_OrbitingParticles  ]-----------------------------------------
struct VisualFX_OrbitingParticles {
  Texture::TextureIndex particle_texture;
  float particle_size;
  D3DCOLOR particle_colors[3];
  float lifetime;   // How long a particle lasts at its current size
  float generation_period; // X particles every this # of seconds
  int particles_per_period;
  float decay_rate; // How quickly particles decay (and disappear) after they die
  float initial_distance;
  float initial_height;
  float rising_speed;
  float spread_speed;
  float orbit_speed;
  bool attach_to_target;
};


}
}

#endif