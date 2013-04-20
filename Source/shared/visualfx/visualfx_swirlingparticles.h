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
#ifndef __VISUALFX_SWIRLINGPARTICLES_H__
#define __VISUALFX_SWIRLINGPARTICLES_H__
#pragma once


#include "shared/texture/textureindex.h"
#include "common/d3dcolor.h"


namespace Evidyon {
namespace VisualFX {

//----[  VisualFX_SwirlingParticles  ]-----------------------------------------
struct VisualFX_SwirlingParticles {
  Texture::TextureIndex particle_texture;
  D3DCOLOR particle_colors[3];
  int number_of_rings;
  float lowest_ring_height, added_height_per_ring;
  int particles_per_ring;
  int particle_trail_length;
  float orbit_radius;
  float min_speed, speed_range;
  float particle_size;
};

}
}


#endif