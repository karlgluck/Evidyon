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
#ifndef __VISUALFX_SWIRLINGPARTICLES_H__
#define __VISUALFX_SWIRLINGPARTICLES_H__


#include <dc/float>
#include <dc/integer>
#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>
#include <dc/boolean>



class VisualFX_SwirlingParticles : public dc::dcResource<VisualFX_SwirlingParticles,VisualFX> {
public:

  // Initializes this class
  VisualFX_SwirlingParticles();

  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference particle_image_;  /// The image for this particle
  dc::dcFloat duration_;
  dc::dcBoolean target_source_;
  dcx::dcxD3DColor particle_colors_[3];  /// The different colors for the particles
  dc::dcInteger number_of_rings_; //
  dc::dcFloat min_ring_height_, max_ring_height_;
  dc::dcInteger particles_per_ring_;
  dc::dcInteger particle_trail_length_;
  dc::dcFloat orbit_radius_;
  dc::dcFloat minimum_speed_, maximum_speed_;
  dc::dcFloat particle_size_;


protected:
  static dc::dcClass<VisualFX_SwirlingParticles>::Implements<VisualFX> implements_visual_fx;
};




#endif