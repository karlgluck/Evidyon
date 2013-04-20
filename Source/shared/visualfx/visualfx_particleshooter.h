//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008, 2009, 2010 Karl Gluck                                  //
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
#ifndef __VISUALFX_PARTICLESHOOTER_H__
#define __VISUALFX_PARTICLESHOOTER_H__
#pragma once

#include "shared/texture/textureindex.h"
#include "common/d3dcolor.h"



namespace Evidyon {
namespace VisualFX {


//----[  VisualFX_ParticleShooter  ]-------------------------------------------
struct VisualFX_ParticleShooter {
  Texture::TextureIndex particle_texture;
  float particle_size;
  D3DCOLOR particle_colors[3];
  float lifetime;
  float generation_period;
  int particles_per_period;
  float initial_height;
  float angle_around_vertical;
  float initial_distance;
  float radial_velocity;
  float decay_rate;
  float gravity, drag;
  bool attach_to_target;
};

}
}


#endif