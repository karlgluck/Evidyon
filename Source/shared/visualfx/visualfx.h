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