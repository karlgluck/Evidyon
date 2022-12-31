//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright © 2001 - 2022 Karl Gluck                                       //
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