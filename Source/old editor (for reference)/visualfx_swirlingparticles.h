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