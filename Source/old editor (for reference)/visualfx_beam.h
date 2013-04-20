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

#ifndef __VISUALFX_BEAM_H__
#define __VISUALFX_BEAM_H__


#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>

class VisualFX_Beam : public dc::dcResource<VisualFX_Beam,VisualFX> {
public:
  VisualFX_Beam();
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference particle_image_;  // The image for the end particles
  dc::dcList<VisualFXImage>::Reference beam_image_;      // The image for the beam itself
  dc::dcFloat particle_size_; // The size of the end particles
  dc::dcFloat duration_;
  dc::dcFloat height_;      // The y-location of the beam and particles
  dc::dcFloat beam_width_;  // Width o' beam

protected:
  static dc::dcClass<VisualFX_Beam>::Implements<VisualFX> implements_visual_fx;
};




#endif