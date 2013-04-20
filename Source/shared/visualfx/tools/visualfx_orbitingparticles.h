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
#ifndef __TOOLS_VISUALFX_ORBITINGPARTICLES_H__
#define __TOOLS_VISUALFX_ORBITINGPARTICLES_H__
#pragma once

#include "visualfx.h"
#include <dc/integer>
#include <dc/float>
#include <dcx/d3dcolor>
#include "shared/texture/tools/texture.h" // for TextureReference


namespace Evidyon {
namespace VisualFX {
namespace Tools {

//----[  VisualFX_OrbitingParticles  ]-----------------------------------------
class VisualFX_OrbitingParticles
  : public dc::dcResource<VisualFX_OrbitingParticles,VisualFX> {
public:
  static dc::dcClass<VisualFX_OrbitingParticles>::Implements<VisualFX> implementsVisualFX;
  static std::string staticTypeName() { return "VisualFX_OrbitingParticles"; }

public:
  VisualFX_OrbitingParticles();
  virtual void compile(Evidyon::VisualFX::VisualFX* description);

private:
  Evidyon::Tools::TextureReference particle_texture_;
  dc::dcFloat particle_size_;   // The size of the particles before they start decaying
  dcx::dcxD3DColor colors_[3];  // Colors
  dc::dcFloat visible_time_;  // Total time a particle is visible
  dc::dcInteger density_;     // number of particles that will exist at a time
  dc::dcInteger particles_per_period_;     // number of particles that are generated simultaneously
  dc::dcFloat decay_time_;    // Portion of visible time spent decaying
  dc::dcFloat initial_distance_;  // Distance away in the ring from the source point
  dc::dcFloat initial_height_;    // The initial Y-location of the particles
  dc::dcFloat rising_speed_;      // How quickly the particles drift upward (negative is downward)
  dc::dcFloat spread_speed_;  // How fast the particles move away from the center
  dc::dcFloat orbit_speed_;   // How fast the particles turn around the central axis
  dc::dcBoolean attach_to_target_; // Either attach to target (true) or source
};


}
}
}

#endif