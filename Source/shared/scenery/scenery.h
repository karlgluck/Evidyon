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
#ifndef __SHARED_SCENERY_SCENERY_H__
#define __SHARED_SCENERY_SCENERY_H__
#pragma once


#include <d3dx9.h> // D3DXMATRIX, D3DXVECTOR3
#include "shared/mesh/meshindex.h"
#include "shared/texture/textureindex.h"
#include "shared/visualfx/visualfxindex.h"


namespace Evidyon {
namespace Scenery {


//----[  Scenery  ]------------------------------------------------------------
struct Scenery {

  // Geometry to render for this scenery
  Mesh::MeshIndex mesh;

  // The number of entries in this array is determined by the mesh type
  Texture::TextureIndex* subset_textures;

  // Local reconfiguration of the geometry.  Multiplied into the combined
  // transform when a scenery instance's location is updated.
  D3DXMATRIX/*A16*/ local_transform;

  // What kind of special effect to attach to this scenery.  This is to allow
  // for cool effects like sparkly swords and burning torches.
  VisualFX::VisualFXIndex visual_fx;
  D3DXVECTOR3 visual_fx_offset;
};



}
}

#endif