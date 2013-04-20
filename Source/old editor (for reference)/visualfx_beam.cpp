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
#include "stdafx.h"
#include "image.h"
#include "wavsound.h"
#include "visualfx_beam.h"
#include "../shared/compiledspecialfx.h"



/// This class implements a special effect
dcClass<VisualFX_Beam>::Implements<VisualFX> VisualFX_Beam::implements_visual_fx;




//------------------------------------------------------------------------------------------------
// Name: VisualFX_Beam
// Desc: Initializes this class
//------------------------------------------------------------------------------------------------
VisualFX_Beam::VisualFX_Beam() {
  member("Particle Image (Ends)", &particle_image_);
  member("Beam Image", &beam_image_);
  member("Particle Size", &particle_size_);
  member("Duration", &duration_);
  member("Height", &height_);
  member("Beam Width", &beam_width_);

}



//------------------------------------------------------------------------------------------------
// Name: compileForClient
// Desc: Enters data into the client effect structure
//------------------------------------------------------------------------------------------------
bool VisualFX_Beam::compileForClient(Evidyon::CompiledVisualFXParams* params) const {
  params->beam.end_particle_sfx_image = particle_image_.getReferencedResourceIndex();
  params->beam.end_particle_size = particle_size_.getValue();
  params->beam.beam_particle_sfx_image = beam_image_.getReferencedResourceIndex();
  params->beam.beam_width = beam_width_.getValue();
  params->beam.height = height_.getValue();
  params->beam.duration = duration_.getValue();
  params->type = Evidyon::CompiledVisualFXParams::BEAM;

  // Success
  return true;
}



//------------------------------------------------------------------------------------------------
// Name: staticTypeName
// Desc: Returns the type name of this class
//------------------------------------------------------------------------------------------------
std::string VisualFX_Beam::staticTypeName() {
    return "VisualFX_Beam";
}


