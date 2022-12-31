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
#ifndef __VISUALFX_H__
#define __VISUALFX_H__
#pragma once



#include "visualfx_orbitingparticles.h"
#include "visualfx_particleshooter.h"
#include "visualfx_swirlingparticles.h"
#include "visualfx_beam.h"
#include "visualfx_scenery.h"
#include "visualfx_grounddecal.h"
#include "visualfx_fireflycloud.h"
#include "visualfx_flickeringlight.h"


namespace Evidyon {
namespace VisualFX {

//----[  VisualFX  ]-----------------------------------------------------------
struct VisualFX {
  enum Type {
    ORBITING_PARTICLES,
    PARTICLE_SHOOTER,
    SWIRLING_PARTICLES,
    BEAM,
    SCENERY,
    GROUND_DECAL,
    FIREFLY_CLOUD,
    FLICKERING_LIGHT,
  } type;
  
  union {
    VisualFX_OrbitingParticles orbiting_particles;
    VisualFX_ParticleShooter particle_shooter;
    VisualFX_SwirlingParticles swirling_particles;
    VisualFX_Beam beam;
    VisualFX_Scenery scenery;
    VisualFX_GroundDecal ground_decal;
    VisualFX_FireflyCloud firefly_cloud;
    VisualFX_FlickeringLight flickering_light;
  };
};

}
}


#endif