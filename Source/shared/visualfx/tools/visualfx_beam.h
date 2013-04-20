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
#ifndef __TOOLS_VISUALFX_BEAM_H__
#define __TOOLS_VISUALFX_BEAM_H__
#pragma once

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/scenery/tools/scenery.h" // for TextureReference


namespace Evidyon {
namespace VisualFX {
namespace Tools {


  
//----[  VisualFX_Beam  ]------------------------------------------------------
class VisualFX_Beam
  : public dc::dcResource<VisualFX_Beam,VisualFX> {
public:
  static dc::dcClass<VisualFX_Beam>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_Beam"; }

public:
  VisualFX_Beam();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  Evidyon::Tools::TextureReference end_particles_texture_;
  dcx::dcxD3DColor end_particles_color_;
  Evidyon::Tools::TextureReference beam_texture_;
  dcx::dcxD3DColor beam_color_;


  // Rescales the beam based on the intensity of the effect
  dc::dcFloat end_particle_size_min_intensity_;
  dc::dcFloat end_particle_size_max_intensity_;
  dc::dcFloat beam_width_min_intensity_;
  dc::dcFloat beam_width_max_intensity_;
};


}
}
}

#endif