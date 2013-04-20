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
#ifndef __ANIMATEDGEOMETRYVERTEX_H__
#define __ANIMATEDGEOMETRYVERTEX_H__
#pragma once



#include "common/d3dcolor.h"

namespace Evidyon {
namespace AnimatedMesh {


#pragma pack(push, 1)

  
//----[  AnimatedGeometryVertex  ]---------------------------------------------
struct AnimatedGeometryVertex {
  float x, y, z;
  float weights[3];
  union {
    unsigned long packed_matrix_indices;
    unsigned char matrix_indices[4];
  };
  //D3DCOLOR diffuse;
  float nx, ny, nz;
  float u, v;
};

#pragma pack(pop)

#define D3DFVF_ANIMATEDGEOMETRYVERTEX (D3DFVF_XYZB4|D3DFVF_LASTBETA_UBYTE4|\
                                       D3DFVF_NORMAL|/*D3DFVF_DIFFUSE|*/D3DFVF_TEX1)



}
}

#endif