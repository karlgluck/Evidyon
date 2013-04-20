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
#ifndef __TOOLS_VISUALFX_FIREFLYCLOUD_H__
#define __TOOLS_VISUALFX_FIREFLYCLOUD_H__
#pragma once

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/texture/tools/texture.h" // for TextureReference


namespace Evidyon {
namespace VisualFX {
namespace Tools {

  
//----[  VisualFX_FireflyCloud  ]----------------------------------------------
class VisualFX_FireflyCloud
  : public dc::dcResource<VisualFX_FireflyCloud,VisualFX> {
public:
  static dc::dcClass<VisualFX_FireflyCloud>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_FireflyCloud"; }

public:
  VisualFX_FireflyCloud();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  Evidyon::Tools::TextureReference particle_texture_;
  dc::dcFloat particle_size_;   // The size of the particles before they start decaying
  dcx::dcxD3DColor colors_[3];  // Colors
  dc::dcFloat visible_time_;  // Total time a particle is visible
  dc::dcInteger density_;     // number of particles that will exist at a time
  dc::dcInteger particles_per_period_;  // number of particles that are generated simultaneously
  dc::dcFloat fading_time_percent_; // Portion of visible time spent fading
  dc::dcFloat distance_range_;
  dc::dcFloat minimum_speed_;
  dc::dcFloat maximum_speed_;
  dc::dcBoolean attach_to_target_; // Either attach to target (true) or source
};


}
}
}

#endif