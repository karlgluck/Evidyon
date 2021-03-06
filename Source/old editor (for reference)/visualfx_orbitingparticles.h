//---------------------------------------------------------------------------//
//  This file is part of Evidyon, a 3d multiplayer online role-playing game  //
//  Copyright � 2008 - 2013 Karl Gluck                                       //
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

#ifndef __VISUALFX_ORBITINGPARTICLES_H__
#define __VISUALFX_ORBITINGPARTICLES_H__


#include <dc/float>
#include <dc/integer>
#include "visualfx.h"
#include "visualfximage.h"
#include <dcx/d3dcolor>
#include <dc/boolean>



class VisualFX_OrbitingParticles : public dc::dcResource<VisualFX_OrbitingParticles,VisualFX> {
public:

  // Initializes this class
  VisualFX_OrbitingParticles();

  // Enters data into the client effect structure
  virtual bool compileForClient(Evidyon::CompiledVisualFXParams* params) const;


public:
  // Returns the type name of this class
  static std::string staticTypeName();


protected:
  dc::dcList<VisualFXImage>::Reference myImage;  /// The image for this particle
  dc::dcFloat myDuration;
  dc::dcBoolean target_source_;
  dcx::dcxD3DColor myParticleColor[3];  /// The different colors for the particles
  dc::dcByte myParticlesPerPeriod;  /// How many particles to create each period
  dc::dcInteger myParticleDensity; // lifetime / [[spawn period]] * particles per period = density
  dc::dcFloat myRisingSpeed;  /// How quickly the particles drift upward (negative is downward)
  dc::dcFloat myInitialDistance;  /// Distance away in the ring from the source point
  dc::dcFloat mySpreadingSpeed; /// How fast the particles move away from the center
  dc::dcFloat myOrbitSpeed; /// How fast the particles turn around the central axis
  dc::dcFloat myTotalLifetime; /// Total lifetime of a particle
  dc::dcFloat myDecayTime;  /// How much of a particle's total lifetime is spent decaying
  dc::dcFloat myParticlesInitialHeight; /// The initial Y-location of the particles
  dc::dcFloat myParticleSize; /// The size of the particles before they start decaying


protected:
  static dc::dcClass<VisualFX_OrbitingParticles>::Implements<VisualFX> implementsVisualFX;
};




#endif