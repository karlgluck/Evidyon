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