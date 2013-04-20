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
#ifndef __VISUALFXINSTANCE_GROUNDDECAL_H__
#define __VISUALFXINSTANCE_GROUNDDECAL_H__
#pragma once



#include "visualfxinstance.h"
#include "common/d3dcolor.h"


namespace Evidyon {
namespace VisualFX {
struct VisualFX_GroundDecal;

  
//----[  VisualFXInstance_GroundDecal  ]---------------------------------------
class VisualFXInstance_GroundDecal : public VisualFXInstance {
  static const int MAX_NUMBER_OF_DECALS = 16;
  struct Decal {
    float x[4], z[4]; // corner coordinates
    float dying_time; // when to start decaying this decal
    D3DCOLOR color;   // decal's color
  };

public:
  VisualFXInstance_GroundDecal(const VisualFX_GroundDecal* parameters);
  virtual void destroy();
  virtual bool update(double time,
                      double time_since_last_update);
  virtual void render(Evidyon::Texture::TextureIndex texture);
private:
  const VisualFX_GroundDecal* parameters_;
  Decal decals_[MAX_NUMBER_OF_DECALS];
  size_t number_of_decals_;
  double next_generate_decal_time_;
};

}
}

#endif