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